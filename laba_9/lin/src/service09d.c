#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "become_daemon.h"

#define BUFFER_SIZE 1024
#define EVENT_BUFFER_SIZE (1024 * (sizeof(struct inotify_event) + 16))

bool dir_exist(const char *dir);
void open_dir_log_file(const char *dir);
void open_srv_log_file(const char *dir);
void write_dir_log(const char *format, ...);
void write_srv_log(const char *format, ...);
void write_dir_log_no_time_prefix(const char *format, ...);
void write_srv_log_no_time_prefix(const char *format, ...);

void monitor_directory_changes(const char *directory_path);
void release_resources();

void sigterm_handler(int signum);
void sighup_handler(int signum);
void setup_signal_handlers();

void daemon_main();
void read_config(const char *config_file, char *log_dir, char *watch_dir);

static const char config_file[]     = "/etc/service09d/service09d.conf";
static const char default_log_dir[] = "/var/log/service09d";

FILE *dir_log_file = NULL;
FILE *srv_log_file = NULL;

atomic_bool reload_config_flag = false;

int main() {
    become_daemon(0);
    daemon_main();
}

void daemon_main() {
    setup_signal_handlers();

reload_config:
    char log_dir[128]   = "\0";
    char watch_dir[128] = "\0";
    read_config(config_file, log_dir, watch_dir);

    if (strlen(log_dir) == 0) strcpy(log_dir, default_log_dir);

    bool log_dir_exist = dir_exist(log_dir);
    if (!log_dir_exist) mkdir(log_dir, 0755);

    open_srv_log_file(log_dir);
    open_dir_log_file(log_dir);

    if (log_dir_exist)
        write_srv_log("Opened already existed log directory '%s'.\n", log_dir);
    else
        write_srv_log("Created new log directory '%s'.\n", log_dir);

    if (!dir_exist(watch_dir)) {
        write_srv_log("Fail. Watch directory does not exist '%s'.\n", watch_dir);
        return;
    }

    write_srv_log(
        "Success. Daemon Service09d started with parameters: <%s>, <%s>.\n", log_dir, watch_dir
    );

    monitor_directory_changes(watch_dir);

    if (atomic_load(&reload_config_flag)) {
        release_resources();
        atomic_store(&reload_config_flag, false);
        goto reload_config;
    }

    write_srv_log("Daemon Service09d stopped...\n");
    release_resources();
}

#include <signal.h>

void sigterm_handler(int signum) {
    write_srv_log("Daemon stopped by signal %d.\n", signum);
    release_resources();
    exit(EXIT_SUCCESS);
}

void sighup_handler(int signum) {
    write_srv_log("SIGHUP event.\n");
    atomic_store(&reload_config_flag, true);
}

void setup_signal_handlers() {
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);
    signal(SIGHUP, sighup_handler);
}

void monitor_directory_changes(const char *directory_path) {
    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        write_srv_log("Failed to initialize inotify: %s\n", strerror(errno));
        return;
    }

    int watch_descriptor = inotify_add_watch(
        inotify_fd, directory_path, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
    );
    if (watch_descriptor < 0) {
        write_srv_log(
            "Failed to add watch for directory %s: %s\n", directory_path, strerror(errno)
        );
        close(inotify_fd);
        return;
    }

    char buffer[EVENT_BUFFER_SIZE];
    write_srv_log("Monitoring directory '%s' ...\n", directory_path);

    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(inotify_fd, &read_fds);

        int ready = select(inotify_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ready < 0) {
            write_srv_log("Failed to select on inotify fd: %s\n", strerror(errno));
            break;
        }

        if (atomic_load(&reload_config_flag)) {
            write_srv_log("Reloading configuration...\n");
            inotify_rm_watch(inotify_fd, watch_descriptor);
            close(inotify_fd);
            return;
        }

        if (FD_ISSET(inotify_fd, &read_fds)) {
            ssize_t length = read(inotify_fd, buffer, EVENT_BUFFER_SIZE);
            if (length < 0) {
                write_srv_log("Failed to read directory changes: %s\n", strerror(errno));
                break;
            }

            ssize_t i = 0;
            while (i < length) {
                struct inotify_event *event = (struct inotify_event *)&buffer[i];
                write_dir_log("Event -> ");

                if (event->mask & IN_CREATE)
                    write_dir_log_no_time_prefix("file or directory created: %s\n", event->name);
                if (event->mask & IN_DELETE)
                    write_dir_log_no_time_prefix("file or directory deleted: %s\n", event->name);
                if (event->mask & IN_MODIFY)
                    write_dir_log_no_time_prefix("file or directory modified: %s\n", event->name);
                if (event->mask & IN_MOVED_FROM)
                    write_dir_log_no_time_prefix("file or directory moved from: %s\n", event->name);
                if (event->mask & IN_MOVED_TO)
                    write_dir_log_no_time_prefix("file or directory moved to: %s\n", event->name);

                i += sizeof(struct inotify_event) + event->len;
            }
        }
    }

    write_srv_log("Stopped monitoring directory '%s' ...\n", directory_path);

    inotify_rm_watch(inotify_fd, watch_descriptor);
    close(inotify_fd);
}

void read_config(const char *config_file, char *log_dir, char *watch_dir) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "log_dir=%s", log_dir) == 1) continue;
        if (sscanf(line, "watch_dir=%s", watch_dir) == 1) continue;
    }

    fclose(file);
}

bool dir_exist(const char *dir) {
    struct stat stats;
    return stat(dir, &stats) == 0 && S_ISDIR(stats.st_mode);
}

void open_log_file(FILE **log_file, const char *dir, const char *suffix) {
    if (*log_file != NULL) return;

    char filepath[BUFFER_SIZE];
    time_t now   = time(NULL);
    struct tm *t = localtime(&now);

    char filename_[128];
    char filename[128];
    snprintf(filename_, sizeof(filename_), "%%Y-%%m-%%d_%%H-%%M-%%S-%s", suffix);
    strftime(filename, sizeof(filename), filename_, t);

    snprintf(filepath, sizeof(filepath), "%s/%s", dir, filename);

    *log_file = fopen(filepath, "w");
    if (*log_file == NULL) {
        fprintf(
            stderr,
            "Failed to create log file %s in directory %s. Error: %s\n",
            suffix,
            dir,
            strerror(errno)
        );
    }
}

void open_dir_log_file(const char *dir) {
    open_log_file(&dir_log_file, dir, "dir.log");
}

void open_srv_log_file(const char *dir) {
    open_log_file(&srv_log_file, dir, "srv.log");
}

void write_log(FILE *log_file, const char *format, va_list args) {
    if (log_file == NULL) return;

    char buffer[BUFFER_SIZE];
    char timestamp[16];

    time_t now   = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", t);

    vsnprintf(buffer, sizeof(buffer), format, args);

    fprintf(log_file, "%s%s", timestamp, buffer);
    fflush(log_file);
}

void write_log_no_time_prefix(FILE *log_file, const char *format, va_list args) {
    if (log_file == NULL) return;

    char buffer[BUFFER_SIZE];
    vsnprintf(buffer, sizeof(buffer), format, args);

    fprintf(log_file, "%s", buffer);
    fflush(log_file);
}

void write_dir_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    write_log(dir_log_file, format, args);
    va_end(args);
}

void write_srv_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    write_log(srv_log_file, format, args);
    va_end(args);
}

void write_dir_log_no_time_prefix(const char *format, ...) {
    va_list args;
    va_start(args, format);
    write_log_no_time_prefix(dir_log_file, format, args);
    va_end(args);
}

void write_srv_log_no_time_prefix(const char *format, ...) {
    va_list args;
    va_start(args, format);
    write_log_no_time_prefix(srv_log_file, format, args);
    va_end(args);
}

void release_resources() {
    if (dir_log_file) fclose(dir_log_file), dir_log_file = NULL;
    if (srv_log_file) fclose(srv_log_file), srv_log_file = NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_BUFFER_SIZE (1024 * (sizeof(struct inotify_event) + 16))

// Функция для вывода содержимого каталога
void PrintDirectoryContents(const char *directoryPath)
{
    struct dirent *entry;
    DIR *dir = opendir(directoryPath);

    if (!dir)
    {
        perror("Failed to open directory");
        return;
    }

    printf("Contents of directory '%s':\n", directoryPath);

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            struct stat st;
            char fullPath[1024];
            snprintf(fullPath, 1024, "%s/%s", directoryPath, entry->d_name);

            if (stat(fullPath, &st) == 0)
            {
                if (S_ISDIR(st.st_mode))
                {
                    printf("[DIR]  %s\n", entry->d_name);
                }
                else
                {
                    printf("[FILE] %s\n", entry->d_name);
                }
            }
        }
    }

    closedir(dir);
}

// Функция для отслеживания изменений в каталоге
void MonitorDirectory(const char *directoryPath)
{
    int inotifyFd = inotify_init();
    if (inotifyFd < 0)
    {
        perror("Failed to initialize inotify");
        return;
    }

    int watchDescriptor = inotify_add_watch(inotifyFd, directoryPath, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
    if (watchDescriptor < 0)
    {
        perror("Failed to add inotify watch");
        close(inotifyFd);
        return;
    }

    printf("Monitoring directory '%s' for changes...\n", directoryPath);

    char buffer[EVENT_BUFFER_SIZE];

    while (1)
    {
        ssize_t length = read(inotifyFd, buffer, EVENT_BUFFER_SIZE);
        if (length < 0)
        {
            perror("Failed to read inotify events");
            break;
        }

        for (char *ptr = buffer; ptr < buffer + length;)
        {
            struct inotify_event *event = (struct inotify_event *)ptr;

            if (event->len > 0)
            {
                if (event->mask & IN_CREATE)
                {
                    printf("File created: %s\n", event->name);
                }
                else if (event->mask & IN_DELETE)
                {
                    printf("File deleted: %s\n", event->name);
                }
                else if (event->mask & IN_MODIFY)
                {
                    printf("File modified: %s\n", event->name);
                }
                else if (event->mask & IN_MOVED_FROM)
                {
                    printf("File moved from: %s\n", event->name);
                }
                else if (event->mask & IN_MOVED_TO)
                {
                    printf("File moved to: %s\n", event->name);
                }
            }

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    inotify_rm_watch(inotifyFd, watchDescriptor);
    close(inotifyFd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    const char *directoryPath = argv[1];

    // Проверка существования каталога
    struct stat st;
    if (stat(directoryPath, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "Directory '%s' does not exist or is not a valid directory.\n", directoryPath);
        return 1;
    }

    // Вывод содержимого каталога
    PrintDirectoryContents(directoryPath);

    // Отслеживание изменений в каталоге
    MonitorDirectory(directoryPath);

    return 0;
}

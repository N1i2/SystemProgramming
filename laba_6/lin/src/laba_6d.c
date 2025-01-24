#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define BORD "================================================================\n"

int printFileInfo(const char *fileName);
int printFileTxt(const char *fileName);
void printFileTime(const char *label, time_t fileTime);
int isTextFile(const char *fileName);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file path>\n", argv[0]);
        return 1;
    }

    const char *PATH_TO_FILE = argv[1];

    printf(BORD);
    if (!printFileInfo(PATH_TO_FILE)) {
        fprintf(stderr, "Information error.\n");
        return 1;
    }
    printf(BORD);

    if (!printFileTxt(PATH_TO_FILE)) {
        fprintf(stderr, "Read error.\n");
        return 1;
    }
    printf(BORD);

    return 0;
}

int printFileInfo(const char *fileName) {
    struct stat fileStat;

    if (stat(fileName, &fileStat) == -1) {
        fprintf(stderr, "Failed to get file information: %s\n", strerror(errno));
        return 0;
    }

    printf("FILE NAME:\n\t%s\n", fileName);

    printf("FILE SIZE:\n\t%lld B\n\t%.2f KiB\n\t%.2f MiB\n",
           (long long)fileStat.st_size,
           fileStat.st_size / 1024.0,
           fileStat.st_size / 1024.0 / 1024.0);

    printf("FILE TYPE:\n\t");
    if (S_ISREG(fileStat.st_mode)) {
        printf("Regular file\n");
    } else if (S_ISDIR(fileStat.st_mode)) {
        printf("Directory\n");
    } else if (S_ISCHR(fileStat.st_mode)) {
        printf("Character device\n");
    } else if (S_ISBLK(fileStat.st_mode)) {
        printf("Block device\n");
    } else if (S_ISFIFO(fileStat.st_mode)) {
        printf("FIFO (named pipe)\n");
    } else if (S_ISLNK(fileStat.st_mode)) {
        printf("Symbolic link\n");
    } else if (S_ISSOCK(fileStat.st_mode)) {
        printf("Socket\n");
    } else {
        printf("Unknown type\n");
    }

    printf("FILE TIME INFORMATION:\n");
    printf("Last file access:         %s", ctime(&fileStat.st_atime));
    printf("Last file modification:   %s", ctime(&fileStat.st_mtime));
    printf("Last status change:       %s", ctime(&fileStat.st_ctime));

    return 1;
}

int printFileTxt(const char *fileName) {
    printf("\nFILE TEXT:\n");

    if (!isTextFile(fileName)) {
        fprintf(stderr, "The specified file is not a text file.\n");
        return 0;
    }

    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        return 0;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    if (bytesRead == -1) {
        fprintf(stderr, "Failed to read file: %s\n", strerror(errno));
        close(fd);
        return 0;
    }

    printf("\nThe file is opened and read.\n");
    close(fd);

    return 1;
}

int isTextFile(const char *fileName) {
    const char *ext = strrchr(fileName, '.');
    if (ext != NULL && strcmp(ext, ".txt") == 0) {
        return 1;
    }
    return 0;
}

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include <string.h>
#include <wctype.h>

#define DEFAULT_FIFO_NAME "/tmp/DefaultPipe"

void toUpperCase(wchar_t *str) {
    while (*str) {
        *str = towupper(*str);
        str++;
    }
}

int main(int argc, char *argv[]) 
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    char *fifoName = argc > 1 ? argv[1] : DEFAULT_FIFO_NAME;

    umask(0);
    if (mkfifo(fifoName, S_IRWXU | S_IRWXG | S_IRWXO) == -1 && errno != EEXIST) {
        perror("Failed to create FIFO");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());

    printf("Server is waiting for connection...\n");

    while (1) 
    {
        int fifo_read = open(fifoName, O_RDONLY);
        if (fifo_read == -1) {
            perror("Failed to open FIFO for reading");
            continue;
        }

        char buffer[256];
        int bytesRead = read(fifo_read, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("Message from client: %s\n", buffer);

            wchar_t wbuffer[256];
            mbstowcs(wbuffer, buffer, sizeof(wbuffer) / sizeof(wbuffer[0]));

            toUpperCase(wbuffer);

            wcstombs(buffer, wbuffer, sizeof(buffer));

            printf("Converting: %s\n", buffer);
            sleep(20);

            int fifo_write = open(fifoName, O_WRONLY);
            write(fifo_write, buffer, strlen(buffer) + 1);
            close(fifo_write);
        }

        close(fifo_read);
    }

    unlink(fifoName);
    return 0;
}
// lab-03b-client
#include <fcntl.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

#define DEFAULT_FIFO_NAME "/tmp/DefaultPipe"
#define DEFAULT_SEMAPHORE_NAME "/DefaultSemaphore"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    char *message = argc > 1 ? argv[1] : "Hello, Сервер!";
    char *fifoName = argc > 2 ? argv[2] : DEFAULT_FIFO_NAME;
    char *semaphoreName = argc > 3 ? argv[3] : DEFAULT_SEMAPHORE_NAME;

    printf("PID: %d\n", getpid());

    sem_t *sem = sem_open(semaphoreName, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("Failed to create semaphore");
        return 1;
    }

    sem_wait(sem);

    int fifo_write = open(fifoName, O_WRONLY);
    if (fifo_write == -1)
    {
        perror("Failed to open FIFO for writing");
        sem_post(sem);
        return 1;
    }

    write(fifo_write, message, strlen(message) + 1);
    close(fifo_write);


    int fifo_read = open(fifoName, O_RDONLY);
    if (fifo_read == -1)
    {
        perror("Failed to open FIFO for reading");
        sem_post(sem);
        return 1;
    }


    char* response[256];
    int bytesRead = read(fifo_read, response, sizeof(response) - 1);
    if (bytesRead > 0)
    {
        response[bytesRead] = '\0';
        printf("Message from server: %s\n", response);
    }

    close(fifo_read);
    sem_post(sem);

    sem_close(sem);

    //unlink(fifoName);
    sem_unlink(semaphoreName);

    return 0;
}
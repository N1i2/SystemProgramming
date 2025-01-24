#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    pid_t pid1, pid2;
    int iterations = 100;  
    char iterations_str[10];

    snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);

    pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        printf("First process (PID: %d)\n", getpid());
        execlp("Lab-02x", "Lab-02x", iterations_str, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    setenv("ITER_NUM", iterations_str, 1);

    pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        printf("Second pocess (PID: %d)\n", getpid());
        execlp("Lab-02x", "Lab-02x", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    wait(NULL);  
    wait(NULL);  

    printf("All correct.\n");

    return 0;
}

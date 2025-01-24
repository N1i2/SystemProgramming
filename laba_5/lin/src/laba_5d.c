#include <stdio.h>
#include <unistd.h>

void sampleFunction() {
    while (1) {
        continue;
    }
}

int global_initialized = 10;
int global_uninitialized;

static int global_static_initialized = 20;
static int global_static_uninitialized;

int main(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());

    int local_initialized = 30;
    int local_uninitialized;

    static int local_static_initialized = 40;
    static int local_static_uninitialized;

    printf("Address of global_initialized: %p\n", (void*)&global_initialized);
    printf("Address of global_uninitialized: %p\n", (void*)&global_uninitialized);
    printf("Address of global_static_initialized: %p\n", (void*)&global_static_initialized);
    printf("Address of global_static_uninitialized: %p\n", (void*)&global_static_uninitialized);

    printf("Address of local_initialized: %p\n", (void*)&local_initialized);
    printf("Address of local_uninitialized: %p\n", (void*)&local_uninitialized);
    printf("Address of local_static_initialized: %p\n", (void*)&local_static_initialized);
    printf("Address of local_static_uninitialized: %p\n", (void*)&local_static_uninitialized);

    printf("Address of sampleFunction: %p\n", (void*)&sampleFunction);
    printf("Address of main: %p\n", (void*)&main);

    printf("Address of argc: %p\n", (void*)&argc);
    printf("Address of argv: %p\n", (void*)&argv);

    printf("Press Enter to exit...\n");
    getchar();

    return 0;
}
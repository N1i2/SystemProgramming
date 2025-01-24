#include <stdio.h>
#include <process.h>

int global_uninit_numb;
int global_init_numb = 1;

static int static_global_uninit_numb;
static int static_global_init_numb = 2;

void SayHello() {
    static int static_local_uninit_numb;
    static int static_local_init_numb = 5;

    printf("Address of static_local_uninit_numb: %p\n", (void*)&static_local_uninit_numb);
    printf("Address of static_local_init_numb: %p\n", (void*)&static_local_init_numb);

    printf("Hello\n");
}

int main(int argc, char* argv[]) {
    printf("PID: %d\n", _getpid());

    int local_uninit_numb;
    int local_init_numb = 3;

    printf("Address of global_init_numb: %p\n", (void*)&global_init_numb);
    printf("Address of global_uninit_numb: %p\n", (void*)&global_uninit_numb);
    printf("Address of static_global_init_numb: %p\n", (void*)&static_global_init_numb);
    printf("Address of static_global_uninit_numb: %p\n", (void*)&static_global_uninit_numb);

    printf("Address of some_function: %p\n", (void*)& SayHello);
    printf("Address of main: %p\n", (void*)&main);

    printf("Address of argc: %p\n", (void*)&argc);
    printf("Address of argv: %p\n", (void*)&argv);

    printf("Address of local_init_numb: %p\n", (void*)&local_init_numb);
    printf("Address of local_uninit_numb: %p\n", (void*)&local_uninit_numb);

    SayHello();

    printf("Press any key to continue...\n");
    getchar();

    return 0;
}

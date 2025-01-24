#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <ctype.h>

int isNumb(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }

    return 1;
}

//int main(int argc, char* argv[]) {
//    int iterations;
//
//    if (argc < 2) {
//        printf("Error with operation.\n");
//        ExitProcess(1);
//    }
//    else if (isNumb(argv[1]) == 0) {
//        printf("Operation is not a number.\n");
//        ExitProcess(2);
//    }
//    else {
//        iterations = atoi(argv[1]);
//    }
//
//    printf("Count of iteration: %d\n", iterations);
//
//    DWORD pid;
//
//    for (int i = 0; i < iterations; i++) {
//        pid = GetCurrentProcessId();
//        printf("%d) PID: %lu\n", i + 1, pid);
//        Sleep(500);
//    }
//
//    return 0;
//}

int main(int argc, char* argv[]) {
    int iterations = 0;

    if (argc < 2) {
        char* env_iter_num = getenv("ITER_NUM");
        if (env_iter_num == NULL) {
            printf("Error: with ITER_NUM.\n");
            ExitProcess(1);
        }
        else {
            if (isNumb(env_iter_num) == 0) {
                printf("Error: ITER_NUM is not a number.\n");
                ExitProcess(2);
            }

            iterations = atoi(env_iter_num);
        }
    }
    else {
        if (isNumb(argv[1]) == 0) {
            printf("Error: argument is not a number.\n");
            ExitProcess(3);
        }

        iterations = atoi(argv[1]);
    }

    printf("Count of iterations: %d\n", iterations);

    DWORD pid;

    for (int i = 0; i < iterations; i++) {
        pid = GetCurrentProcessId();
        printf("%d) PID: %lu\n", i + 1, pid);
        Sleep(500);
    }

    system("pause");
    return 0;
}

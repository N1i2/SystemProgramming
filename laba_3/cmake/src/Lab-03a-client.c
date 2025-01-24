#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isPrime(int number) {
    if (number <= 1) return 0;
    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) return 0;
    }
    return 1;
}

void findPrimesAndSend(int lower, int upper, HANDLE pipe, HANDLE mutex) {
    char buffer[1024];
    int position = 0;
    
    for (int i = lower; i <= upper; ++i) {
        if (isPrime(i)) {
            position += snprintf(buffer + position, sizeof(buffer) - position, "%d || ", i);
        }
    }

    WaitForSingleObject(mutex, INFINITE);
    DWORD written;
    WriteFile(pipe, buffer, strlen(buffer), &written, NULL);
    ReleaseMutex(mutex);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: Lab-03a-client <lower> <upper> [mutex_name]\n");
        return 1;
    }

    Sleep(10000);

    char bufferName[256] = "Global\\defaultName";
    DWORD resultEnvName = GetEnvironmentVariable("MUTEX_ENV_NAME", bufferName, sizeof(bufferName));
    if(resultEnvName > 0 && resultEnvName < 256){
        printf("Global Variable Name: %s\n", bufferName);
    } else if(resultEnvName == 0){
         printf("Global Variable Name not set, now Name: %s\n", bufferName);
    } else {
         printf("Buffer size so small");
    }

    int lower = atoi(argv[1]);
    int upper = atoi(argv[2]);
    char* mutexName = (argc >= 4) ? argv[3] : bufferName;

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
    if (mutex == NULL) {
        fprintf(stderr, "Failed to open mutex.\n");
        return 1;
    }

    //дискриптор стандартного вывода процесса в кансоли
    HANDLE pipe = GetStdHandle(STD_OUTPUT_HANDLE);
    if(pipe == INVALID_HANDLE_VALUE){
        fprintf(stderr, "Failed to get pipe handle.\n");
        CloseHandle(mutex);
        return 1;
    }

    findPrimesAndSend(lower, upper, pipe, mutex);

    CloseHandle(pipe);
    CloseHandle(mutex);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void CreateChildProcess(int lower, int upper, const char *mutex_name, HANDLE pipe)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = pipe;

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "D:\\Univer\\SystemProgramming\\laba_3\\cmake\\build\\Lab-03a-client.exe %d %d %s", lower, upper, mutex_name);

    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        fprintf(stderr, "Failed to create process.\n");
    }

    //Sleep(10000);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <num_processes> <lower> <upper> [mutex_name]\n", argv[0]);
        return 1;
    }

    char bufferName[256] = "Global\\defaultName";
    DWORD resultEnvName = GetEnvironmentVariable("MUTEX_ENV_NAME", bufferName, sizeof(bufferName));
    if (resultEnvName > 0 && resultEnvName < 256)
    {
        printf("Global Variable Name: %s\n", bufferName);
    }
    else if (resultEnvName == 0)
    {
        printf("Global Variable Name not set, now Name: %s\n", bufferName);
    }
    else
    {
        printf("Buffer size so small");
    }

    int num_processes = atoi(argv[1]);
    int lower = atoi(argv[2]);
    int upper = atoi(argv[3]);
    const char *mutex_name = (argc >= 5) ? argv[4] : bufferName;

    int range = (upper - lower + 1) / num_processes;

    HANDLE mutex = CreateMutex(NULL, FALSE, mutex_name);
    if (mutex == NULL)
    {
        fprintf(stderr, "Failed to create mutex.\n");
        return 1;
    }

    HANDLE hWritePipe, hReadPipe;
    SECURITY_ATTRIBUTES sa;

    // устанавливает атрибуты защиты канала
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL; // защита по умолчанию
    sa.bInheritHandle = TRUE;       // дескрипторы наследуемые
    if (!PipeCreate(&hWritePipe, &hReadPipe, &sa, 0))
    {
        fprintf(stderr, "Failed to create pipe.\n");
        CloseHandle(mutex);
        return 1;
    }

    for (int i = 0; i < num_processes; ++i)
    {
        int sub_lower = lower + i * range;
        int sub_upper = (i == num_processes - 1) ? upper : sub_lower + range - 1;
        CreateChildProcess(sub_lower, sub_upper, mutex_name, hReadPipe);
    }

    char buffer[1024];
    DWORD read;
    for (int i = 0; i < num_processes; ++i)
    {
        ReadFile(hWritePipe, buffer, sizeof(buffer) - 1, &read, NULL);
        buffer[read] = '\0';
        printf("%s", buffer);
        //Sleep(5000);
    }

    // while (ReadFile(hWritePipe, buffer, sizeof(buffer) - 1, &read, NULL))
    // {
    //     buffer[read] = '\0';
    //     printf("%s", buffer);
    // }

    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);

    CloseHandle(mutex);


    return 0;
}

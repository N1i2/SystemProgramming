#include <windows.h>
#include <stdio.h>
#include <locale.h>

#define DEFAULT_PIPE_NAME L"\\\\.\\pipe\\DefaultPipe"
#define DEFAULT_SEMAPHORE_NAME L"Global\\DefaultSemaphore"

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    wchar_t pipeNameBuff[256] = DEFAULT_PIPE_NAME;
    DWORD resultEnvNamePipe = GetEnvironmentVariableW(L"PIPE_ENV_NAME", pipeNameBuff, sizeof(pipeNameBuff) / sizeof(wchar_t));
    if(resultEnvNamePipe > 0 && resultEnvNamePipe < 256){
        wprintf(L"Global Variable Name: %s\n", pipeNameBuff);
    } else if(resultEnvNamePipe == 0){
         wprintf(L"Global Variable Name not set, now Name: %s\n", pipeNameBuff);
    } else {    
         wprintf(L"Buffer size so small");
    }

    wchar_t semaphoreNameBuff[256] = DEFAULT_SEMAPHORE_NAME;
    DWORD resultEnvNameSem = GetEnvironmentVariableW(L"SEMAPHORE_ENV_NAME", semaphoreNameBuff, sizeof(semaphoreNameBuff) / sizeof(wchar_t));
    if(resultEnvNameSem > 0 && resultEnvNameSem < 256){
        wprintf(L"Global Variable Name: %s\n\n", semaphoreNameBuff);
    } else if(resultEnvNameSem == 0){
         wprintf(L"Global Variable Name not set, now Name: %s\n\n", semaphoreNameBuff);
    } else {
         wprintf(L"Buffer size so small\n\n");
    }

    wchar_t *message = argc > 1 ? argv[1] : L"Hello, Сервер!";
    wchar_t *pipeName = argc > 2 ? argv[2] : pipeNameBuff;
    wchar_t *semaphoreName = argc > 3 ? argv[3] : semaphoreNameBuff;

    HANDLE hSemaphore = CreateSemaphoreW(NULL, 1, 1, DEFAULT_SEMAPHORE_NAME);
    if (hSemaphore == NULL)
    {
        fwprintf(stderr, L"Failed to create semaphore. Error: %lu\n", GetLastError());
        return 1;
    }

    WaitForSingleObject(hSemaphore, INFINITE);

    HANDLE hPipe;
    if ((hPipe = CreateFileW(
             pipeName,
             GENERIC_READ | GENERIC_WRITE,
             0,
             NULL,
             OPEN_EXISTING,
             0,
             NULL)) == INVALID_HANDLE_VALUE)
    {
        fwprintf(stderr, L"Failed to connect to the channel. Error: %lu\n", GetLastError());
        ReleaseSemaphore(hSemaphore, 1, NULL);
        return 1;
    }

    DWORD bytesWritten;
    if (!WriteFile(hPipe, message, (wcslen(message) + 1) * sizeof(wchar_t), &bytesWritten, NULL)) {
        fwprintf(stderr, L"Failed to send message. Error: %lu\n", GetLastError());
        CloseHandle(hPipe);
        ReleaseSemaphore(hSemaphore, 1, NULL);
        return 1;
    }

    wchar_t buffRes[256];
    DWORD bytesRead;
    if(!ReadFile(hPipe, buffRes,sizeof(buffRes), &bytesRead, NULL)){
        fprintf(stderr, "not sent");
        CloseHandle(hPipe);
        ReleaseSemaphore(hSemaphore, 1 ,NULL);
        return 1;
    }

    wprintf(L"Response: %s\n", buffRes);

    CloseHandle(hPipe);
    ReleaseSemaphore(hSemaphore, 1, NULL);
    return 0;
}
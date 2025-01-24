#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <locale.h>

#define DEFAULT_PIPE_NAME L"\\\\.\\pipe\\DefaultPipe"
#define DEFAULT_SEMAPHORE_NAME L"Global\\DefaultSemaphore"

void toUpperCase(wchar_t *str) {
    while (*str) {
        *str = towupper(*str);
        str++;
    }
}

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    HANDLE hPipe;
    wchar_t *pipeName = argc > 1 ? argv[1] : DEFAULT_PIPE_NAME;

    wchar_t pipeNameBuff[256] = DEFAULT_PIPE_NAME;
    DWORD resultEnvNamePipe = GetEnvironmentVariableW(L"PIPE_ENV_NAME", pipeNameBuff, sizeof(pipeNameBuff) / sizeof(wchar_t));
    if(resultEnvNamePipe > 0 && resultEnvNamePipe < 256){
        wprintf(L"Global Variable Name: %s\n\n", pipeNameBuff);
    } else if(resultEnvNamePipe == 0){
         wprintf(L"Global Variable Name not set, now Name: %s\n\n", pipeNameBuff);
    } else {
         wprintf(L"Buffer size so small\n\n");
    }

    if ((hPipe = CreateNamedPipeW(
             pipeName,
             PIPE_ACCESS_DUPLEX,
             PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
             PIPE_UNLIMITED_INSTANCES,
             256 * sizeof(wchar_t),
             256 * sizeof(wchar_t),
             INFINITE,
             NULL)) == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Failed to create channel. Error: %lu\n", GetLastError());
        return 1;
    }

    wprintf(L"Server is waiting for connection...\n");

    while (1) {
        if (!ConnectNamedPipe(hPipe, NULL)) {
            wprintf(L"Error connecting to channel. Error: %lu\n", GetLastError());
            CloseHandle(hPipe);
            return 1;
        }

        wchar_t buffer[256];
        DWORD bytesRead;
        if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
            wprintf(L"Error reading from channel. Error: %lu\n", GetLastError());
            DisconnectNamedPipe(hPipe);
            continue;
        }

        wprintf(L"Message from client: %s\n", buffer);

        toUpperCase(buffer);
        Sleep(5000);

        wprintf(L"Converting: %s\n\n", buffer);

        DWORD bytesWritten;
        if (!WriteFile(hPipe, buffer, (wcslen(buffer) + 1) * sizeof(wchar_t), &bytesWritten, NULL)) {
            wprintf(L"Error sending response. Error: %lu\n", GetLastError());
            DisconnectNamedPipe(hPipe);
            continue;
        }

        DisconnectNamedPipe(hPipe);
    }

    CloseHandle(hPipe);
    return 0;
}
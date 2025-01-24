#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char processNames[5][MAX_PATH] = {
        "Lab-02hw-1.exe",
        "Lab-02hw-2.exe",
        "Lab-02hw-3.exe",
        "Lab-02hw-4.exe",
        "Lab-02hw-5.exe"
    };

    if (!SetCurrentDirectory("D:\\Univer\\SystemProgramming\\laba_2\\Castom")) {
        printf("Error with custom. Error: %d\n", GetLastError());
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        char commandLine[MAX_PATH];
        sprintf_s(commandLine, MAX_PATH, "%s", processNames[i]);

        if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("Error with proccess %s. Error %d\n", processNames[i], GetLastError());
        }
        else {
            printf("Proccess create.\n");

            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    system("pause");
    return 0;
}

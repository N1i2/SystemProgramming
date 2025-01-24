#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>

void createProcessWithFirstParam(LPCSTR programName, int iterations);
void createProcessWithSecondParam(LPCSTR programName, int iterations);
void createProcessWithAllParams(LPCSTR programName, int iterations);

int main() {
    LPCSTR programName = "D:\\Univer\\SystemProgramming\\laba_2\\laba_2\\x64\\Debug\\Lab-02x.exe";
    int iterations1 = 3;
    int iterations2 = 4;
    int iterations3 = 5;

    createProcessWithFirstParam(programName, iterations1);
    createProcessWithSecondParam(programName, iterations2);
    createProcessWithAllParams(programName, iterations3);

    system("pause");
    return 0;
}

void createProcessWithFirstParam(LPCSTR programName, int iterations) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char commandLine[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    sprintf(commandLine, "%s %d", programName, iterations);

    if (!CreateProcess(commandLine, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create first process. Error: %lu\n", GetLastError());
        return;
    }

    printf("First process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void createProcessWithSecondParam(LPCSTR programName, int iterations) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char commandLine[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    sprintf(commandLine, "%s %d", programName, iterations);

    if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create second process. Error: %lu\n", GetLastError());
        return;
    }

    printf("Second process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void createProcessWithAllParams(LPCSTR programName, int iterations) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char commandLine[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    sprintf(commandLine, "%s %d", programName, iterations);

    if (!CreateProcess(programName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create third process. Error: %lu\n", GetLastError());
        return;
    }

    printf("Third process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

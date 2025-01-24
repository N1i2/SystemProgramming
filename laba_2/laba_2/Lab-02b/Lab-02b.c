#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>

void createProcessWithFileOnly(LPCSTR programName);
void createProcessWithNullCommandLine(LPCSTR programName);
void createProcessWithLocalEnvironment(LPCSTR programName);

int main() {
    LPCSTR programName = "D:\\Univer\\SystemProgramming\\laba_2\\laba_2\\x64\\Debug\\Lab-02x.exe";

    SetEnvironmentVariable("ITER_NUM", "5");

    createProcessWithFileOnly(programName);
    createProcessWithNullCommandLine(programName);
    createProcessWithLocalEnvironment(programName);

    return 0;
}

void createProcessWithFileOnly(LPCSTR programName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(programName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create first process. Error: %lu\n", GetLastError());
        return;
    }

    printf("First process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void createProcessWithNullCommandLine(LPCSTR programName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char commandLine[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    sprintf(commandLine, "%s", programName);

    if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create second process. Error: %lu\n", GetLastError());
        return;
    }

    printf("Second process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void createProcessWithLocalEnvironment(LPCSTR programName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char commandLine[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    SetEnvironmentVariable("ITER_NUM", "9");
    sprintf(commandLine, "%s", programName);

    if (!CreateProcess(programName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create third process. Error: %lu\n", GetLastError());
        return;
    }

    printf("Third process created (PID: %lu)\n", pi.dwProcessId);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
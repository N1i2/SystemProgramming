#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>

void createProcessWithFileOnly(LPCSTR programName);
void createProcessWithNullCommandLine(LPCSTR programName, int iterations);
void createProcessWithLocalEnvironment(LPCSTR programName, int localIterations);

int main() {
    LPCSTR programName = "D:\\Univer\\SystemProgramming\\laba_2\\laba_2\\x64\\Debug\\Lab-02x.exe";
    int globalIterations = 5;

    SetEnvironmentVariable("ITER_NUM", "5");

    createProcessWithFileOnly(programName);

    createProcessWithNullCommandLine(programName, globalIterations);

    int localIterations = 10;
    createProcessWithLocalEnvironment(programName, localIterations);

    system("pause");
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

void createProcessWithNullCommandLine(LPCSTR programName, int iterations) {
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

void createProcessWithLocalEnvironment(LPCSTR programName, int localIterations) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LPVOID envBlock = GetEnvironmentStrings();
    char newEnv[256];
    sprintf(newEnv, "ITER_NUM=%d", localIterations);
    SetEnvironmentVariable("ITER_NUM", newEnv); 

    if (!CreateProcess(programName, NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, envBlock, NULL, &si, &pi)) {
        printf("Failed to create third process. Error: %lu\n", GetLastError());
        return;
    }

    printf("Third process created (PID: %lu) with local ITER_NUM = %d\n", pi.dwProcessId, localIterations);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
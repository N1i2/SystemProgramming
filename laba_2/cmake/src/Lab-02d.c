#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD pid;
    char f;
    const char* infFile = "Lab-02inf.exe";

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, infFile, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Second process. Error: %d\n", GetLastError());
        system("pause");
        return 1;
    }

    printf("Second process correct. PID: %d\n", pi.dwProcessId);
    printf("Press if you want stop...\n");
    f = getchar();

    if (!TerminateProcess(pi.hProcess, 1)) {
        printf("Error with end. Error: %d\n", GetLastError());
    }
    else {
        printf("Second process end.\n");
    }

    printf("Enter ...\n");
    f = getchar();

    pid = GetProcessId(pi.hProcess);
    if (pid == 0) {
        printf("PID. Error: %d\n", GetLastError());
    }
    else {
        printf("PID: %d\n", pid);
    }

    printf("Press and close window...\n");
    f = getchar();

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    pid = GetProcessId(pi.hProcess);
    if (pid == 0) {
        printf("After CloseHandle: PID Error: %d\n", GetLastError());
    }
    else {
        printf("After CloseHandle: PID: %d\n", pid);
    }

    printf("End Task...\n");
    f = getchar(); 

    system("pause");
    return 0;
}

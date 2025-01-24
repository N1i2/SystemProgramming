#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <direct.h>
#include <time.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

char service_name[] = "laba_9a";

SERVICE_STATUS service_status;
SERVICE_STATUS_HANDLE hServiceStatus;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
void WriteLog(const char *filePath, const char *format, ...);

HANDLE hDir = NULL;
char srv_log_file[MAX_PATH];
char dir_log_file[MAX_PATH];
char monitored_dir[MAX_PATH];
HANDLE hStopEvent = NULL;
HANDLE hPauseEvent = NULL;

#define DEFAULT_MONITORED_DIR "D:\\Univer\\SystemProgramming\\laba_9\\test"
#define DEFAULT_LOG_DIR "D:\\Univer\\SystemProgramming\\laba_9\\winLog"

int CreateDirectoriesRecursively(const char *dir)
{
    if (PathFileExists(dir))
    {
        return 0; // Directory already exists
    }

    char temp[MAX_PATH];
    strcpy(temp, dir);

    for (char *p = temp + 1; *p; p++)
    {
        if (*p == '\\' || *p == '/')
        {
            *p = '\0';
            if (!PathFileExists(temp))
            {
                if (!CreateDirectory(temp, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    return -1;
                }
            }
            *p = '\\';
        }
    }

    if (!CreateDirectory(temp, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    SERVICE_TABLE_ENTRY service_table[] = {
        {service_name, ServiceMain},
        {NULL, NULL}};

    if (!StartServiceCtrlDispatcher(service_table))
    {
        printf("Failed to start service control dispatcher.\n");
        return 1;
    }

    return 0;
}

void WriteLog(const char *filePath, const char *format, ...)
{
    FILE *file = fopen(filePath, "a");
    if (!file)
    {
        return;
    }

    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    fprintf(file, "[%02d:%02d:%02d] ", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);

    fclose(file);
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    hServiceStatus = RegisterServiceCtrlHandler(service_name, ServiceCtrlHandler);
    if (!hServiceStatus)
    {
        return;
    }

    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_START_PENDING;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    service_status.dwWin32ExitCode = NO_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 5000;

    SetServiceStatus(hServiceStatus, &service_status);

    // Если переданы параметры, используйте их
    if (dwArgc >= 3)
    {
        strcpy(monitored_dir, lpszArgv[1]);
        sprintf(dir_log_file, "%s\\%lld-dir.log", lpszArgv[2], time(NULL));
        sprintf(srv_log_file, "%s\\%lld-srv.log", lpszArgv[2], time(NULL));
    }
    else
    {
        strcpy(monitored_dir, DEFAULT_MONITORED_DIR);
        sprintf(dir_log_file, "%s\\%lld-dir.log", DEFAULT_LOG_DIR, time(NULL));
        sprintf(srv_log_file, "%s\\%lld-srv.log", DEFAULT_LOG_DIR, time(NULL));
    }

    // Проверка и создание директорий
    if (CreateDirectoriesRecursively(monitored_dir) != 0)
    {
        WriteLog("D:\\Univer\\SystemProgramming\\laba_9\\service_error.log", "Failed to create monitored directory: %s.\n", monitored_dir);
        service_status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    if (CreateDirectoriesRecursively(lpszArgv[2]) != 0)
    {
        WriteLog("D:\\Univer\\SystemProgramming\\laba_9\\service_error.log", "Failed to create log directory: %s.\n", lpszArgv[2]);
        service_status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    WriteLog(srv_log_file, "Service started. Monitoring: %s, Logs: %s\n", monitored_dir, lpszArgv[2]);
    service_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &service_status);

    hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    hPauseEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    hDir = CreateFile(
        monitored_dir,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (hDir == INVALID_HANDLE_VALUE)
    {
        WriteLog(srv_log_file, "Failed to open directory for monitoring: %s\n", monitored_dir);
        service_status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION *notify;

    while (WaitForSingleObject(hStopEvent, 0) != WAIT_OBJECT_0)
    {
        WaitForSingleObject(hPauseEvent, INFINITE);

        if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL))
        {
            notify = (FILE_NOTIFY_INFORMATION *)buffer;
            if (service_status.dwCurrentState == SERVICE_PAUSED)
            {
                continue;
            }
            do
            {

                char fileName[MAX_PATH];
                int count = WideCharToMultiByte(
                    CP_ACP, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR),
                    fileName, sizeof(fileName) - 1, NULL, NULL);
                fileName[count] = '\0';

                switch (notify->Action)
                {
                case FILE_ACTION_ADDED:
                    WriteLog(dir_log_file, "File created: %s\n", fileName);
                    break;
                case FILE_ACTION_REMOVED:
                    WriteLog(dir_log_file, "File deleted: %s\n", fileName);
                    break;
                case FILE_ACTION_MODIFIED:
                    WriteLog(dir_log_file, "File modified: %s\n", fileName);
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    WriteLog(dir_log_file, "File renamed: old name: %s\n", fileName);
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    WriteLog(dir_log_file, "File renamed: new name: %s\n", fileName);
                    break;
                default:
                    WriteLog(dir_log_file, "Unknown file action: %d on file %s\n", notify->Action, fileName);
                    break;
                }

                if (notify->NextEntryOffset == 0)
                {
                    break;
                }

                notify = (FILE_NOTIFY_INFORMATION *)((char *)notify + notify->NextEntryOffset);
            } while ((char *)notify < buffer + bytesReturned);
        }
    }

    CloseHandle(hDir);
    WriteLog(srv_log_file, "Service stopped.\n");
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
        WriteLog(srv_log_file, "Service is stopping...\n");
        service_status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &service_status);
        SetEvent(hStopEvent);
        service_status.dwCurrentState = SERVICE_STOPPED;
        WriteLog(srv_log_file, "Service stopped successfully.\n");
        break;

    case SERVICE_CONTROL_PAUSE:
        WriteLog(srv_log_file, "Service is pausing...\n");
        service_status.dwCurrentState = SERVICE_PAUSED;
        SetServiceStatus(hServiceStatus, &service_status);
        ResetEvent(hPauseEvent);
        WriteLog(srv_log_file, "Service paused successfully.\n");
        break;

    case SERVICE_CONTROL_CONTINUE:
        WriteLog(srv_log_file, "Service is resuming...\n");
        service_status.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(hServiceStatus, &service_status);
        SetEvent(hPauseEvent);
        WriteLog(srv_log_file, "Service resumed successfully.\n");
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        WriteLog(srv_log_file, "Service is shutting down...\n");
        SetEvent(hStopEvent);
        service_status.dwCurrentState = SERVICE_STOPPED;
        WriteLog(srv_log_file, "Service shut down successfully.\n");
        break;

    default:
        WriteLog(srv_log_file, "Received unknown control code: %d\n", dwControl);
        break;
    }

    SetServiceStatus(hServiceStatus, &service_status);
}

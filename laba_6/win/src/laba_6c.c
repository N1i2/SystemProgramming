#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void PrintDirectoryContents(const char *directoryPath);
void MonitorDirectory(const char *directoryPath);

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    const char *directoryPath = argv[1];

    DWORD fileAttributes = GetFileAttributes(directoryPath);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES || !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        fprintf(stderr, "Directory '%s' does not exist or is not a valid directory.\n", directoryPath);
        return 1;
    }

    PrintDirectoryContents(directoryPath);

    MonitorDirectory(directoryPath);

    return 0;
}

void PrintDirectoryContents(const char *directoryPath)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", directoryPath);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to list directory contents. Error code: %ld\n", GetLastError());
        return;
    }

    printf("Contents of directory '%s':\n", directoryPath);
    do
    {
        const char *name = findFileData.cFileName;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
        {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("[DIR] %s\n", name);
            }
            else
            {
                printf("[FILE] %s\n", name);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void MonitorDirectory(const char *directoryPath)
{
    HANDLE hDir = CreateFile(
        directoryPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (hDir == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Failed to open directory for monitoring. Error code: %ld\n", GetLastError());
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;

    printf("Monitoring directory '%s' for changes...\n", directoryPath);

    while (1)
    {
        if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                TRUE, // Рекурсивное отслеживание
                FILE_NOTIFY_CHANGE_FILE_NAME |
                    FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_LAST_WRITE |
                    FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL))
        {
            FILE_NOTIFY_INFORMATION *notify = (FILE_NOTIFY_INFORMATION *)buffer;
            do
            {
                char fileName[MAX_PATH];
                int fileNameLength = WideCharToMultiByte(CP_UTF8, 0, notify->FileName, notify->FileNameLength / sizeof(WCHAR), fileName, MAX_PATH, NULL, NULL);
                fileName[fileNameLength] = '\0';

                switch (notify->Action)
                {
                case FILE_ACTION_ADDED:
                    printf("File added: %s\n", fileName);
                    break;
                case FILE_ACTION_REMOVED:
                    printf("File removed: %s\n", fileName);
                    break;
                case FILE_ACTION_MODIFIED:
                    printf("File modified: %s\n", fileName);
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    printf("File renamed from: %s\n", fileName);
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    printf("File renamed to: %s\n", fileName);
                    break;
                default:
                    printf("Unknown action on file: %s\n", fileName);
                    break;
                }

                notify = (FILE_NOTIFY_INFORMATION *)((char *)notify + notify->NextEntryOffset);
            } while (notify->NextEntryOffset != 0);
        }
        else
        {
            fprintf(stderr, "Failed to monitor directory. Error code: %ld\n", GetLastError());
            break;
        }
    }

    CloseHandle(hDir);
}
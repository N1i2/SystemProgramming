#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define bord "\n============================================================================\n"

void PrintInfo(LPSTR FileName);
void PrintText(LPSTR FileName);
bool IsTextFile(LPSTR FileName);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("You need to provide a file name\n");
        return 1;
    }

    LPSTR fileName = argv[1];

    printf(bord);
    PrintInfo(fileName);
    printf(bord);

    PrintText(fileName);
    printf(bord);

    return 0;
}

void PrintInfo(LPSTR FileName)
{
    HANDLE hFile = CreateFileA(
        FileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error: Cannot access file %s\n", FileName);
        return;
    }

    BY_HANDLE_FILE_INFORMATION fileInfo;
    if (!GetFileInformationByHandle(hFile, &fileInfo))
    {
        printf("Error: Cannot retrieve file information for %s\n", FileName);
        CloseHandle(hFile);
        return;
    }

    LARGE_INTEGER fileSize;
    fileSize.LowPart = fileInfo.nFileSizeLow;
    fileSize.HighPart = fileInfo.nFileSizeHigh;

    SYSTEMTIME creationTime, accessTime, writeTime;
    FileTimeToSystemTime(&fileInfo.ftCreationTime, &creationTime);
    FileTimeToSystemTime(&fileInfo.ftLastAccessTime, &accessTime);
    FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &writeTime);

    printf("File Name: %s\n", FileName);
    printf("File Size: %lld bytes (%.2f KiB, %.2f MiB)\n",
           fileSize.QuadPart, fileSize.QuadPart / 1024.0, fileSize.QuadPart / (1024.0 * 1024));

    DWORD dwFileType;
    dwFileType = GetFileType(hFile);

    switch (dwFileType)
    {
    case FILE_TYPE_DISK:
        fputs("Disk type file.\n", stdout);
        break;
    case FILE_TYPE_CHAR:
        fputs("Char type file.\n", stdout);
        break;
    case FILE_TYPE_PIPE:
        fputs("Pipe type file.\n", stdout);
        break;
    default:
        fputs("Unknown type file.\n", stdout);
        break;
    }

    printf("Creation Time: %02d/%02d/%04d %02d:%02d:%02d\n",
           creationTime.wDay, creationTime.wMonth, creationTime.wYear,
           creationTime.wHour, creationTime.wMinute, creationTime.wSecond);

    printf("Last Access Time: %02d/%02d/%04d %02d:%02d:%02d\n",
           accessTime.wDay, accessTime.wMonth, accessTime.wYear,
           accessTime.wHour, accessTime.wMinute, accessTime.wSecond);

    printf("Last Write Time: %02d/%02d/%04d %02d:%02d:%02d\n",
           writeTime.wDay, writeTime.wMonth, writeTime.wYear,
           writeTime.wHour, writeTime.wMinute, writeTime.wSecond);

    if (!IsTextFile(FileName))
    {
        printf("The file is binary.\n");
    }
    else
    {
        printf("The file is a text file.\n");
    }

    CloseHandle(hFile);
}

void PrintText(LPSTR FileName)
{
    if (!IsTextFile(FileName))
    {
        printf("Error: The file %s is not a text file.\n", FileName);
        return;
    }

    HANDLE hFile = CreateFileA(
        FileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error: Cannot open file %s\n", FileName);
        return;
    }

    char buffer[1024];
    DWORD bytesRead;

    while (ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    CloseHandle(hFile);
}

bool IsTextFile(LPSTR FileName)
{
    HANDLE hFile = CreateFileA(
        FileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    unsigned char buffer[512];
    DWORD bytesRead;

    if (!ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);

    for (DWORD i = 0; i < bytesRead; ++i)
    {
        if (buffer[i] == 0)
        {
            return false;
        }
    }

    return true;
}

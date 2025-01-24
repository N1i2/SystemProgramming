#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HANDLE hFile = INVALID_HANDLE_VALUE;
char *fileBuffer = NULL;
char **fileRows = NULL;
char *fileNames = NULL;
int rowCount = 0;
size_t fileBufferSize = 0;

BOOL MyOpenFile(LPSTR filePath);
BOOL AddRow(LPSTR row, INT pos);
BOOL RemRow(INT pos);
BOOL PrintRow(INT pos);
BOOL PrintRows();
BOOL CloseFile();
BOOL ReadFileStr();
void DisplayMenu();
void DisplayError(LPSTR errMessage);

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    char filePath[MAX_PATH];
    char rowBuffer[1024];
    int choice;
    int pos;

    system("cls");

    while (1)
    {
        DisplayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            DisplayError("Invalid input. Please enter a valid number.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Enter file path: ");
            scanf("%s", filePath);
            if (!MyOpenFile(filePath))
                DisplayError("Failed to open file.\n");
            break;

        case 2:
            printf("Enter row: ");
            getchar();
            fgets(rowBuffer, sizeof(rowBuffer), stdin);
            rowBuffer[strcspn(rowBuffer, "\n")] = '\0';
            printf("Enter position: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n");
                break;
            }
            if (!AddRow(rowBuffer, pos))
                DisplayError("Failed to add row.\n");
            break;

        case 3:
            printf("Enter position to remove: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n");
                break;
            }
            if (!RemRow(pos))
                DisplayError("Failed to remove row.\n");
            break;

        case 4:
            printf("Enter position to print: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n");
                break;
            }
            if (!PrintRow(pos))
                DisplayError("Failed to print row.\n");
            break;

        case 5:
            if (!PrintRows())
                DisplayError("Failed to print rows.\n");
            break;

        case 6:
            if (!CloseFile())
                DisplayError("Failed to close file.\n");
            break;

        case 0:
            if (hFile != INVALID_HANDLE_VALUE)
                CloseFile();
            return 0;

        default:
            DisplayError("Invalid choice. Please try again.\n");
            break;
        }

        system("cls");
    }
}

void DisplayMenu()
{
    printf("\nCurrent File: %s \n", fileNames ? fileNames : "None");
    printf("\nMenu:\n");
    printf("1. Open File\n");
    printf("2. Add Row\n");
    printf("3. Remove Row\n");
    printf("4. Print Row\n");
    printf("5. Print All Rows\n");
    printf("6. Close File\n");
    printf("0. Exit\n");
}

BOOL ReadFileStr()
{
    DWORD bytesRead;

    if (fileRows)
    {
        free(fileRows);
        fileRows = NULL;
    }

    if (fileBuffer)
    {
        free(fileBuffer);
        fileBuffer = NULL;
    }

    rowCount = 0;

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed to get file size. Error code: %ld\n", GetLastError());
        DisplayError(errorMessage);
        // fprintf(stderr, "Failed to get file size. Error code: %ld\n", GetLastError());
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    fileBufferSize = (size_t)fileSize.QuadPart;
    fileBuffer = (char *)malloc(fileBufferSize + 1);
    if (!fileBuffer)
    {
        DisplayError("Failed to allocate memory for file buffer.\n");
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, fileBuffer, (DWORD)fileBufferSize, &bytesRead, NULL))
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed to read file contents. Error code: %ld\n", GetLastError());
        DisplayError(errorMessage);
        // fprintf(stderr, "Failed to read file contents. Error code: %ld\n", GetLastError());
        free(fileBuffer);
        fileBuffer = NULL;
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    fileBuffer[bytesRead] = '\0';

    char *token = strtok(fileBuffer, "\n");

    while (token)
    {
        fileRows = (char **)realloc(fileRows, (rowCount + 1) * sizeof(char *));
        if (fileRows == NULL)
        {
            free(fileBuffer);
            return FALSE;
        }
        fileRows[rowCount] = token;
        rowCount++;
        token = strtok(NULL, "\n");
    }

    return TRUE;
}

BOOL MyOpenFile(LPSTR filePath)
{
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DisplayError("A file is already open. Close it first.\n");
        return FALSE;
    }

    hFile = CreateFile(
        filePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed to open file. Error code: %ld\n", GetLastError());
        DisplayError(errorMessage);
        return FALSE;
    }

    // читаем строки и записываем в массив
    if (!ReadFileStr())
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Failed to read file contents.\n");
        return FALSE;
    }

    fileNames = filePath;
    return TRUE;
}

BOOL AddRow(LPSTR row, INT pos)
{
    if (hFile == INVALID_HANDLE_VALUE || !row)
    {
        DisplayError("Invalid file handle or row data.\n");
        return FALSE;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = rowCount + pos;
    }

    if (insertPos > rowCount || insertPos < 0)
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Invalid file handle or row data.\n");
        return FALSE;
    }

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    DWORD bytesWritten = 0;
    for (int i = 0; i <= rowCount; i++)
    {
        if (i == insertPos && pos >= 0)
        {
            if (!WriteFile(hFile, row, strlen(row), &bytesWritten, NULL) ||
                !WriteFile(hFile, "\n", 1, &bytesWritten, NULL))
            {
                free(fileBuffer);
                free(fileRows);
                return FALSE;
            }
        }
        if (i < rowCount)
        {
            if (!WriteFile(hFile, fileRows[i], strlen(fileRows[i]), &bytesWritten, NULL) ||
                !WriteFile(hFile, "\n", 1, &bytesWritten, NULL))
            {
                free(fileBuffer);
                free(fileRows);
                return FALSE;
            }
        }
        if (i == insertPos && pos < 0)
        {
            if (!WriteFile(hFile, row, strlen(row), &bytesWritten, NULL) ||
                !WriteFile(hFile, "\n", 1, &bytesWritten, NULL))
            {
                free(fileBuffer);
                free(fileRows);
                return FALSE;
            }
        }
    }

    if (!ReadFileStr())
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Failed to read file contents.\n");
        return FALSE;
    }

    return TRUE;
}

BOOL RemRow(INT pos)
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DisplayError("Invalid file handle.\n");
        return FALSE;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = rowCount + pos;
    }

    if (insertPos >= rowCount || insertPos < 0)
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Invalid file handle or row data.\n");
        return FALSE;
    }

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    DWORD bytesWritten = 0;
    for (int i = 0; i <= rowCount; i++)
    {
        if (i == insertPos)
        {
            continue;
        }
        if (i < rowCount)
        {
            if (!WriteFile(hFile, fileRows[i], strlen(fileRows[i]), &bytesWritten, NULL) ||
                !WriteFile(hFile, "\n", 1, &bytesWritten, NULL))
            {
                free(fileBuffer);
                free(fileRows);
                return FALSE;
            }
        }
    }

    if (!ReadFileStr())
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Failed to read file contents.\n");
        return FALSE;
    }

    return TRUE;
}

BOOL PrintRow(INT pos)
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DisplayError("Invalid file handle.\n");
        return FALSE;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = rowCount + pos;
    }

    if (insertPos >= rowCount || insertPos < 0)
    {
        free(fileBuffer);
        free(fileRows);
        DisplayError("Invalid file handle or row data.\n");
        return FALSE;
    }

    printf("%-2d. %s\n", insertPos + 1, fileRows[insertPos]);
    system("pause");
    return TRUE;
}

BOOL PrintRows()
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DisplayError("Invalid file handle.\n");
        return FALSE;
    }

    fprintf(stdout, "\nFile Contents:\n");
    for (int i = 0; i < rowCount; i++)
    {
        printf("%-2d. %s\n", i + 1, fileRows[i]);
    }
    system("pause");
    return TRUE;
}

BOOL CloseFile()
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DisplayError("Invalid file handle.\n");
        return FALSE;
    }

    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    if (fileRows)
    {
        free(fileRows);
        fileRows = NULL;
    }

    if (fileBuffer)
    {
        free(fileBuffer);
        fileBuffer = NULL;
    }

    fileNames = NULL;
    rowCount = 0;
    fileBufferSize = 0;
    return TRUE;
}

void DisplayError(LPSTR errMessage)
{
    fprintf(stderr, "\n[ERROR] %s \n", errMessage);
    system("pause");
}

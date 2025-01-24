#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "static-lib.h"

// Глобальные переменные
HANDLE hFileMapping = NULL;
struct Student *studentList = NULL;
int listSize = 0;
char *fileName = NULL;

void DisplayMenu()
{
    printf("\nCurrent File: %s \n", fileName ? fileName : "None");
    printf("\nMenu:\n");
    printf("1. Open Mapping\n");
    printf("2. Add Row\n");
    printf("3. Remove Row\n");
    printf("4. Print Row\n");
    printf("5. Print All Rows\n");
    printf("6. Close Mapping\n");
    printf("0. Exit\n");
}

int WriteStudentInfo()
{
    struct Student s;
    int pos;

    printf("Enter rows: \n");

    // Ввод имени
    printf("Введите имя (до 63 символов): ");
    if (scanf("%63s", s.Name) != 1)
    {
        DisplayError("Ошибка ввода имени.", PAUSE_NOEX);
        return 0;
    }

    // Ввод фамилии
    printf("Введите фамилию (до 127 символов): ");
    if (scanf("%127s", s.Surname) != 1)
    {
        DisplayError("Ошибка ввода фамилии.", PAUSE_NOEX);
        return 0;
    }

    // Ввод курса
    printf("Введите курс (от 1 до 5): ");
    if (scanf("%hhu", &s.Course) != 1 || s.Course < 1 || s.Course > 5)
    {
        DisplayError("Ошибка ввода курса. Курс должен быть от 1 до 5.", PAUSE_NOEX);
        return 0;
    }

    // Ввод группы
    printf("Введите группу (от 1 до 99): ");
    if (scanf("%hhu", &s.Group) != 1 || s.Group < 1 || s.Group > 99)
    {
        DisplayError("Ошибка ввода группы. Группа должна быть от 1 до 99.", PAUSE_NOEX);
        return 0;
    }

    // Ввод номера студенческого
    printf("Введите номер студенческого (до 7 символов): ");
    if (scanf("%7s", s.ID) != 1)
    {
        DisplayError("Ошибка ввода номера студенческого.", PAUSE_NOEX);
        return 0;
    }

    printf("Введите позицию: ");
    scanf("%d", &pos);

    if (!AddRow(s, pos))
    {
        DisplayError("Failed to add row.\n", PAUSE_NOEX);
        return 0;
    }

    return 1;
}

int IsMappingOpen()
{
    if (hFileMapping == NULL || studentList == NULL)
    {
        return 0;
    }
    return 1;
}

char *ClearPath(const char *input)
{
    static char output[260];
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] != '\\' && input[i] != '/' && input[i] != ':' && input[i] != '.')
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';

    return output;
}

int OpenMapping(LPSTR filePath, LPINT listSize)
{
    if (IsMappingOpen())
    {
        printf("Mapping already open.");
        return 0;
    }

    HANDLE hFile = CreateFile(
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
        sprintf(errorMessage, "Failed to open file. Error code: %ld", GetLastError());
        DisplayError(errorMessage, PAUSE_NOEX);
        return 0;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed to get file size. Error code: %ld", GetLastError());
        DisplayError(errorMessage, PAUSE_NOEX);
        CloseHandle(hFile);
        return 0;
    }

    if (fileSize.QuadPart == 0)
    {
        *listSize = 10; // По умолчанию размер списка
        fileSize.QuadPart = sizeof(struct Student) * (*listSize);
        SetFilePointer(hFile, fileSize.QuadPart, NULL, FILE_BEGIN);
        SetEndOfFile(hFile);
    }
    else
    {
        *listSize = fileSize.QuadPart / sizeof(struct Student);
    }

    hFileMapping = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        fileSize.QuadPart,
        ClearPath(filePath));
    if (hFileMapping == NULL)
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed creating/opening mapping. Error code: %ld", GetLastError());
        DisplayError(errorMessage, PAUSE_NOEX);
        return 0;
    }
    CloseHandle(hFile);

    studentList = (struct Student *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (studentList == NULL)
    {
        char errorMessage[256];
        sprintf(errorMessage, "Failed memory mapping. Error code: %ld", GetLastError());
        DisplayError(errorMessage, PAUSE_NOEX);
        CloseHandle(hFileMapping);
        return 0;
    }

    // printf("\nMapping open successfully.\n");
    // system("pause");

    fileName = filePath;

    return 1;
}

int AddRow(struct Student row, int pos)
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = listSize + pos;
    }

    // Проверка допустимости позиции
    if (insertPos < 0 || insertPos >= listSize)
    {
        DisplayError("Ошибка: Позиция вне допустимого диапазона.", PAUSE_NOEX);
        return 0;
    }

    // Проверка, занята ли позиция
    if (strlen(studentList[insertPos].ID) != 0)
    {
        DisplayError("Ошибка: Позиция уже занята.", PAUSE_NOEX);
        return 0;
    }

    studentList[insertPos] = row;
    printf("Student added.\n");
    system("pause");

    return 1;
}

int RemRow(int pos)
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = listSize + pos;
    }

    // Проверка допустимости позиции
    if (insertPos < 0 || insertPos >= listSize)
    {
        DisplayError("Ошибка: Позиция вне допустимого диапазона.", PAUSE_NOEX);
        return 0;
    }

    // Проверка, занята ли позиция
    if (strlen(studentList[insertPos].ID) == 0)
    {
        DisplayError("Ошибка: Позиция пуста.", PAUSE_NOEX);
        return 0;
    }

    memset(&studentList[insertPos], 0, sizeof(struct Student));
    printf("\nStudent removed.\n");
    system("pause");

    return 1;
}

int PrintRow(int pos)
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos < 0)
    {
        insertPos = listSize + pos;
    }

    // Проверка допустимости позиции
    if (insertPos < 0 || insertPos >= listSize)
    {
        DisplayError("Ошибка: Позиция вне допустимого диапазона.", PAUSE_NOEX);
        return 0;
    }

    // Проверка, занята ли позиция
    if (strlen(studentList[insertPos].ID) == 0)
    {
        DisplayError("Ошибка: Позиция пуста.", PAUSE_NOEX);
        return 0;
    }

    printf("Имя: %s\nФамилия: %s\nКурс: %d\nГруппа: %d\nНомер студенческого: %s\n",
           studentList[insertPos].Name,
           studentList[insertPos].Surname,
           studentList[insertPos].Course,
           studentList[insertPos].Group,
           studentList[insertPos].ID);

    // system("pause");

    return 1;
}

int PrintRows()
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    int isEmpty = 0;
    for (int i = 0; i < listSize; i++)
    {
        if (strlen(studentList[i].Name) == 0)
        {
            isEmpty++;
        }
    }

    if (isEmpty == listSize)
    {
        printf("\nThe list of Students is empty.\n");
        system("pause");
        return 1;
    }

    for (int i = 1; i <= listSize; i++)
    {
        if (strlen(studentList[i - 1].ID) != 0)
        {
            printf("[%d]\n", i);
            PrintRow(i);
        }
    }

    system("pause");

    return 1;
}

int CloseMapping()
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    UnmapViewOfFile(studentList);
    CloseHandle(hFileMapping);

    hFileMapping = NULL;
    studentList = NULL;

    printf("\nMapping is closed.\n");
    system("pause");

    fileName = NULL;

    return 1;
}

void DisplayError(LPSTR errMessage, enum Pause currentPause)
{
    fprintf(stderr, "\n[ERROR] %s ", errMessage);
    if (currentPause == PAUSE_EX)
    {
        system("pause");
    }
}

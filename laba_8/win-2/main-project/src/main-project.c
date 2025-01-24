#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// #include "dynamic-def-lib.h"

enum Pause
{
    PAUSE_EX,
    PAUSE_NOEX
};

typedef int (*OpenMappingFunc)(const char*, int*);
typedef int (*WriteStudentInfoFunc)();
typedef int (*DisplayMenuFunc)();
typedef int (*RemRowFunc)(int);
typedef int (*PrintRowFunc)(int);
typedef int (*PrintRowsFunc)();
typedef int (*IsMappingOpenFunc)();
typedef int (*CloseMappingFunc)();

typedef int (*DisplayErrorFunc)(LPSTR errMessage, enum Pause currentPause);

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    char filePath[MAX_PATH];
    char rowBuffer[1024];
    int choice;
    int pos;

    HMODULE hLib1 = LoadLibraryA("dynamic-def-lib.dll");
    if (!hLib1) {
        return 1;
    }

    HMODULE hLib2 = LoadLibraryA("dynamic-lib.dll");
    if (!hLib2) {
        FreeLibrary(hLib1);
        return 1;
    }

    DisplayMenuFunc DisplayMenu = (DisplayMenuFunc)GetProcAddress(hLib1, (LPCSTR)1);
    OpenMappingFunc OpenMapping = (OpenMappingFunc)GetProcAddress(hLib1, (LPCSTR)2);
    WriteStudentInfoFunc WriteStudentInfo = (WriteStudentInfoFunc)GetProcAddress(hLib1, (LPCSTR)3);
    RemRowFunc RemRow = (RemRowFunc)GetProcAddress(hLib1, (LPCSTR)4);
    PrintRowFunc PrintRow = (PrintRowsFunc)GetProcAddress(hLib1, (LPCSTR)5);
    PrintRowsFunc PrintRows = (PrintRowsFunc)GetProcAddress(hLib1, (LPCSTR)6);
    CloseMappingFunc CloseMapping = (CloseMappingFunc)GetProcAddress(hLib1, (LPCSTR)7);
    IsMappingOpenFunc IsMappingOpen = (IsMappingOpenFunc)GetProcAddress(hLib1, "IsMappingOpen");


    DisplayErrorFunc DisplayError = (DisplayErrorFunc)GetProcAddress(hLib2, "DisplayError");

    system("cls");

    while (1)
    {
        DisplayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            DisplayError("Invalid input. Please enter a valid number.\n", PAUSE_EX);
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }

        switch (choice)
        {
        case 1:
            if (IsMappingOpen())
            {
                DisplayError("Mapping is already open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            printf("Enter file path: ");
            scanf("%s", filePath);
            if (!OpenMapping(filePath, 0))
                DisplayError("Failed to open file.\n", PAUSE_EX);
            break;

        case 2:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            if (!WriteStudentInfo())
                DisplayError("Failed to add row.\n", PAUSE_EX);
            break;

        case 3:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            printf("Enter position to remove: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n", PAUSE_EX);
                break;
            }
            if (!RemRow(pos))
                DisplayError("Failed to remove row.\n", PAUSE_EX);
            break;

        case 4:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            printf("Enter position to print: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n", PAUSE_EX);
                break;
            }
            if (!PrintRow(pos))
                DisplayError("Failed to print row.\n", PAUSE_EX);
            system("pause");
            break;

        case 5:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            if (!PrintRows())
                DisplayError("Failed to print rows.\n", PAUSE_EX);
            break;

        case 6:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                system("pause");
                break;
            }

            if (!CloseMapping())
                DisplayError("Failed to close file.\n", PAUSE_EX);
            break;

        case 0:
            if (IsMappingOpen())
                CloseMapping();
            printf("\nExit complite...\n");
            system("pause & cls");
            return 0;

        default:
            DisplayError("Invalid choice. Please try again.\n", PAUSE_EX);
            system("pause & cls");
        }

        system("cls");
    }
}
#define _CRT_SECURE_NO_WARNINGS
#include "static-lib.h"

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
            DisplayError("Invalid input. Please enter a valid number.\n", PAUSE_EX);
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Enter file path: ");
            scanf("%s", filePath);
            if (!OpenMapping(filePath, &listSize))
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
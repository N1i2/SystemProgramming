#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Структура студента
struct Student
{
    char Name[64];        // Имя
    char Surname[128];    // Фамилия
    unsigned char Course; // Курс
    unsigned char Group;  // Группа
    char ID[8];           // Номер студенческого
};

enum Pause
{
    PAUSE_EX,
    PAUSE_NOEX
};

// Глобальные переменные
int fd = -1;
struct Student *studentList = NULL;
int listSize = 0;
char *fileName = NULL;

int OpenMapping(const char *filePath, int *listSize);
int AddRow(struct Student row, int pos);
int RemRow(int pos);
int PrintRow(int pos);
int PrintRows();
int CloseMapping();
int ReadFileStr();
int IsMappingOpen();
int WriteStudentInfo();
void DisplayMenu();
void DisplayError(const char *errMessage, enum Pause currentPause);

int main()
{
    // SetConsoleOutputCP(CP_UTF8);
    // SetConsoleCP(CP_UTF8);

    printf("PID: %d", getpid());
    char filePath[1024];
    char rowBuffer[1024];
    int choice;
    int pos;

    system("clear");

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
                getchar();
                break;
            }

            if (!WriteStudentInfo())
                DisplayError("Failed to add row.\n", PAUSE_EX);
                getchar();
            break;
                getchar();
        case 3:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                getchar();
                break;
            }

            printf("Enter position to remove: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n", PAUSE_EX);
                getchar();
                break;
            }
            if (!RemRow(pos))
                DisplayError("Failed to remove row.\n", PAUSE_EX);
                getchar();
            break;

        case 4:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                getchar();
                break;
            }

            printf("Enter position to print: ");
            if (scanf("%d", &pos) != 1)
            {
                DisplayError("Invalid position.\n", PAUSE_EX);
                getchar();
                break;
            }
            if (!PrintRow(pos))
                DisplayError("Failed to print row.\n", PAUSE_EX);
                getchar();
            break;

        case 5:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                getchar();
                break;
            }

            if (!PrintRows())
                DisplayError("Failed to print rows.\n", PAUSE_EX);
                getchar();
            break;

        case 6:
            if (!IsMappingOpen())
            {
                DisplayError("Mapping is not open.", PAUSE_NOEX);
                getchar();
                break;
            }

            if (!CloseMapping())
                DisplayError("Failed to close file.\n", PAUSE_EX);
                getchar();
            break;

        case 0:
            if (IsMappingOpen())
                CloseMapping();
                printf("\nExit complite...\n");
                getchar();
            return 0;

        default:
            DisplayError("Invalid choice. Please try again.\n", PAUSE_EX);
            getchar();
            system("clear");
        }

        system("clear");
    }
}

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
    if (fd == -1 || studentList == NULL)
    {
        return 0;
    }
    return 1;
}

int OpenMapping(const char *filePath, int *listSize)
{
    if (IsMappingOpen())
    {
        printf("Mapping already open.");
        return 0;
    }

    fd = open(filePath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        DisplayError("Failed to open file.", PAUSE_NOEX);
        return 0;
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1)
    {
        perror("Failed to get file size");
        close(fd);
        fd = -1;
        return 0;
    }

    if (fileStat.st_size == 0)
    {
        *listSize = 10; // По умолчанию размер списка
        size_t fileSize = sizeof(struct Student) * (*listSize);
        if (ftruncate(fd, fileSize) == -1)
        {
            perror("Failed to set file size");
            close(fd);
            fd = -1;
            return 0;
        }
    }
    else
    {
        *listSize = fileStat.st_size / sizeof(struct Student);
    }

    studentList = mmap(NULL, sizeof(struct Student) * (*listSize), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (studentList == MAP_FAILED)
    {
        perror("Failed memory mapping");
        close(fd);
        fd = -1;
        return 0;
    }

    fileName = strdup(filePath);

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
    getchar();

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
    getchar();

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

    getchar();

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
        getchar();
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

    system("read -p 'Нажмите любую клавишу, чтобы продолжить...' -n 1");

    return 1;
}

int CloseMapping()
{
    if (!IsMappingOpen())
    {
        DisplayError("Mapping is not open.", PAUSE_NOEX);
        return 0;
    }

    if (munmap(studentList, sizeof(struct Student) * listSize) == -1)
    {
        perror("Failed to unmap memory");
        return 0;
    }

    close(fd);
    fd = -1;
    studentList = NULL;

    free(fileName);
    fileName = NULL;


    printf("\nMapping is closed.\n");


    return 1;
}

void DisplayError(const char *errMessage, enum Pause currentPause)
{
    fprintf(stderr, "\n[ERROR] %s ", errMessage);
    if (currentPause == PAUSE_EX)
    {
        getchar();
    }
}

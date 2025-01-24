#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int fd = -1;
char *fileBuffer = NULL;
char **fileRows = NULL;
char *fileName = NULL;
int rowCount = 0;
size_t fileBufferSize = 0;

int MyOpenFile(const char *filePath);
int AddRow(const char *row, int pos);
int RemRow(int pos);
int PrintRow(int pos);
int PrintRows(void);
int CloseFile(void);
int ReadFileStr(void);
void DisplayMenu(void);
void DisplayError(const char *errMessage);

int main() {
    char filePath[1024];
    char rowBuffer[1024];
    int choice;
    int pos;
    printf("PID: %d", getpid());

    while (1) {
        DisplayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            DisplayError("Invalid input. Please enter a valid number.");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
        case 1:
            printf("Enter file path: ");
            scanf("%s", filePath);
            if (!MyOpenFile(filePath))
                DisplayError("Failed to open file.");
            break;

        case 2:
            printf("Enter row: ");
            getchar();
            fgets(rowBuffer, sizeof(rowBuffer), stdin);
            rowBuffer[strcspn(rowBuffer, "\n")] = '\0';
            printf("Enter position: ");
            if (scanf("%d", &pos) != 1) {
                DisplayError("Invalid position.");
                break;
            }
            if (!AddRow(rowBuffer, pos))
                DisplayError("Failed to add row.");
            break;

        case 3:
            printf("Enter position to remove: ");
            if (scanf("%d", &pos) != 1) {
                DisplayError("Invalid position.");
                break;
            }
            if (!RemRow(pos))
                DisplayError("Failed to remove row.");
            break;

        case 4:
            printf("Enter position to print: ");
            if (scanf("%d", &pos) != 1) {
                DisplayError("Invalid position.");
                break;
            }
            if (!PrintRow(pos))
                DisplayError("Failed to print row.");
            break;

        case 5:
            if (!PrintRows())
                DisplayError("Failed to print rows.");
            break;

        case 6:
            if (!CloseFile())
                DisplayError("Failed to close file.");
            break;

        case 0:
            if (fd != -1)
                CloseFile();
            return 0;

        default:
            DisplayError("Invalid choice. Please try again.");
        }
    }
}

void DisplayMenu(void) {
    printf("\nCurrent File: %s \n", fileName ? fileName : "None");
    printf("\nMenu:\n");
    printf("1. Open File\n");
    printf("2. Add Row\n");
    printf("3. Remove Row\n");
    printf("4. Print Row\n");
    printf("5. Print All Rows\n");
    printf("6. Close File\n");
    printf("0. Exit\n");
}

int ReadFileStr(void) {
    if (fileRows) {
        free(fileRows);
        fileRows = NULL;
    }

    if (fileBuffer) {
        free(fileBuffer);
        fileBuffer = NULL;
    }

    rowCount = 0;

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        perror("Failed to get file size");
        CloseFile();
        return 0;
    }

    fileBufferSize = fileStat.st_size;
    fileBuffer = malloc(fileBufferSize + 1);
    if (!fileBuffer) {
        DisplayError("Failed to allocate memory for file buffer.");
        CloseFile();
        return 0;
    }

    if (pread(fd, fileBuffer, fileBufferSize, 0) == -1) {
        perror("Failed to read file");
        free(fileBuffer);
        fileBuffer = NULL;
        CloseFile();
        return 0;
    }

    fileBuffer[fileBufferSize] = '\0';

    char *token = strtok(fileBuffer, "\n");
    while (token) {
        fileRows = realloc(fileRows, (rowCount + 1) * sizeof(char *));
        if (!fileRows) {
            free(fileBuffer);
            return 0;
        }
        fileRows[rowCount++] = token;
        token = strtok(NULL, "\n");
    }

    return 1;
}

int MyOpenFile(const char *filePath) {
    if (fd != -1) {
        DisplayError("A file is already open. Close it first.");
        return 0;
    }

    fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    if (!ReadFileStr()) {
        DisplayError("Failed to read file contents.");
        return 0;
    }

    fileName = strdup(filePath);
    return 1;
}

int AddRow(const char *row, int pos) {
    if (fd == -1 || !row) {
        DisplayError("Invalid file descriptor or row data.");
        return 0;
    }

    if (pos > rowCount + 1 || pos < -1) {
        DisplayError("Invalid position.");
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos == -1)
    {
        insertPos = rowCount;
    }

    FILE *file = fopen(fileName, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return 0;
    }

    for (int i = 0; i <= rowCount; i++) {
        if (i == insertPos) {
            fprintf(file, "%s\n", row);
        }
        if (i < rowCount) {
            fprintf(file, "%s\n", fileRows[i]);
        }
    }

    fclose(file);

    return ReadFileStr();
}

int RemRow(int pos) {
    if (fd == -1) {
        DisplayError("Invalid file descriptor.");
        return 0;
    }

    if (pos > rowCount || pos < -1) {
        DisplayError("Invalid position.");
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos == -1)
    {
        insertPos = rowCount - 1;
    }

    FILE *file = fopen(fileName, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return 0;
    }

    for (int i = 0; i < rowCount; i++) {
        if (i != insertPos) {
            fprintf(file, "%s\n", fileRows[i]);
        }
    }

    fclose(file);

    return ReadFileStr();
}

int PrintRow(int pos) {
    if (fd == -1 || pos < -1 || pos > rowCount) {
        DisplayError("Invalid position or file descriptor.");
        return 0;
    }

    int insertPos = 0;
    if (pos > 0)
    {
        insertPos = pos - 1;
    }
    else if (pos == -1)
    {
        insertPos = rowCount-1;
    }

    printf("%-2d. %s\n", insertPos + 1, fileRows[insertPos]);
    return 1;
}

int PrintRows(void) {
    if (fd == -1 ) {
        DisplayError("Invalid File handle.");
        return 0;
    }

    fprintf(stdout, "\nFile Contents:\n");
    for (int i = 0; i < rowCount; i++)
    {
        printf("%-2d. %s\n", i + 1, fileRows[i]);
    }
    
    return 1;
}

int CloseFile()
{
    if (fd == -1 ) {
        DisplayError("Invalid File descriptor.");
        return 0;
    }

    if (close(fd) == -1) 
    {
        DisplayError("Failed to close file.");
        return -1;
    }
    fd = -1;

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

    fileName = NULL;
    rowCount = 0;
    fileBufferSize = 0;

    return 0; 
}

void DisplayError(const char * errMessage)
{
    fprintf(stderr, "\n[ERROR] %s \n", errMessage);
}   
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifndef LIBRARY_H
#define LIBRARY_H

// Student structure
struct Student
{
    char Name[64];
    char Surname[128];
    unsigned char Course;
    unsigned char Group;
    char ID[8];
};

// Enumeration for pause handling
enum Pause
{
    PAUSE_EX,
    PAUSE_NOEX
};

// Глобальные переменные
// DLL_API int fd;
// DLL_API struct Student *studentList;
// DLL_API int listSize;
// DLL_API char *fileName;

// Exported functions
int OpenMapping(const char *filePath, int listSizeDef);
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

#endif // LIBRARY_H
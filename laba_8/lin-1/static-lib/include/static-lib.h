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
extern int fd;
extern struct Student *studentList;
extern int listSize;
extern char *fileName;

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

#endif // LIBRARY_H

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
extern HANDLE hFileMapping;
extern struct Student *studentList;
extern int listSize;
extern char *fileName;

int OpenMapping(LPSTR filePath, LPINT listSize);
int AddRow(struct Student row, int pos);
int RemRow(int pos);
int PrintRow(int pos);
int PrintRows();
int CloseMapping();
int ReadFileStr();
int IsMappingOpen();
int WriteStudentInfo();
void DisplayMenu();
void DisplayError(LPSTR errMessage, enum Pause currentPause);
char *ClearPath(const char *input);

#endif // LIBRARY_H

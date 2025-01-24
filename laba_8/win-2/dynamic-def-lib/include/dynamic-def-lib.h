#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define DLL_API for export
#ifdef DLL_API

#else

#define DLL_API extern __declspec(dllexport)
#endif

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

// // Глобальные переменные
DLL_API HANDLE hFileMapping;
DLL_API struct Student *studentList;
DLL_API int listSize;
DLL_API char *fileName;

// Exported functions
DLL_API int OpenMapping(LPSTR filePath, int listSizeDef);
DLL_API int AddRow(struct Student row, int pos);
DLL_API int RemRow(int pos);
DLL_API int PrintRow(int pos);
DLL_API int PrintRows();
DLL_API int CloseMapping();
DLL_API int WriteStudentInfo();
DLL_API int IsMappingOpen();
DLL_API void DisplayMenu();
DLL_API void DisplayError(LPSTR errMessage, enum Pause currentPause);
DLL_API char *ClearPath(const char *input);

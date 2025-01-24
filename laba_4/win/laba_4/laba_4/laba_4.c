#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

CRITICAL_SECTION cs;
DWORD dwTlsIndex;
int* globalArray = NULL;
int globalCount = 0;

int isPrime(int num)
{
    if (num <= 1)
        return 0;
    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

VOID ErrorExit(LPCSTR message)
{
    fprintf(stderr, "%s\n", message);
    ExitProcess(0);
}

DWORD WINAPI L4Thread(LPVOID param)
{
    int* range = (int*)param;
    int start = range[0];
    int end = range[1];

    free(range);

    int* localStorageData = (int*)LocalAlloc(LPTR, (end - start) * sizeof(int));
    if (localStorageData == NULL)
    {
        printf("TLS error \n");
        return 1;
    }

    if (!TlsSetValue(dwTlsIndex, localStorageData))
    {
        printf("Error: TlsSetValue\n");
        LocalFree(localStorageData);
        return 1;
    }

    int localCount = 0;

    for (int num = start; num <= end; num++)
    {
        if (isPrime(num))
        {
            localStorageData[localCount++] = num;
        }
    }

    //Sleep(30000);

    EnterCriticalSection(&cs);
    int* retrievedArray = (int*)TlsGetValue(dwTlsIndex);
    if (retrievedArray != NULL)
    {
        for (int i = 0; i < localCount; i++)
        {
            globalArray[globalCount++] = retrievedArray[i];
        }
    }

    LocalFree(localStorageData);
    LeaveCriticalSection(&cs);
    return 0;
}

int compareInt(const void* a, const void* b)
{
    int intA = *(int*)a;
    int intB = *(int*)b;
    return (intA - intB);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Error: need start with parameters: <num_processes> <lower> <upper>\n");
        return 1;
    }

    int numThreads = atoi(argv[1]);
    int minBound = atoi(argv[2]);
    int maxBound = atoi(argv[3]);

    if (numThreads <= 0 || minBound > maxBound)
    {
        printf("Error: invalid parameters\n");
        return 1;
    }

    InitializeCriticalSection(&cs);

    globalArray = (int*)malloc((maxBound - minBound + 1) * sizeof(int));
    if (globalArray == NULL)
    {
        printf("Error: Failed to allocate memory for globalPrimes\n");
        DeleteCriticalSection(&cs);
        return 1;
    }

    int rangeSize = (maxBound - minBound + 1) / numThreads;
    int remainder = (maxBound - minBound + 1) % numThreads;

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL)
    {
        printf("Error: Failed to allocate memory for threads or thread ranges\n");
        free(globalArray);
        DeleteCriticalSection(&cs);
        return 1;
    }

    if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
        ErrorExit("TlsAlloc failed");

    for (int i = 0; i < numThreads; i++)
    {
        int local_min = minBound + i * rangeSize;
        int local_max = local_min + rangeSize - 1;

        if (i == numThreads - 1)
        {
            local_max += remainder;
        }

        int* threadRanges = (int*)malloc(2 * sizeof(int));
        threadRanges[0] = local_min;
        threadRanges[1] = local_max;

        threads[i] = CreateThread(NULL,
            0,
            (LPTHREAD_START_ROUTINE)L4Thread,
            threadRanges,
            0,
            NULL);

        if (threads[i] == NULL)
        {
            printf("Error: Failed to create thread\n");
            free(threadRanges);
            free(globalArray);
            free(threads);
            free(threadRanges);
            DeleteCriticalSection(&cs);
            TlsFree(dwTlsIndex);
            return 1;
        }
    }

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    for (int i = 0; i < numThreads; i++)
    {
        CloseHandle(threads[i]);
    }

    EnterCriticalSection(&cs);
    qsort(globalArray, globalCount, sizeof(int), compareInt);
    LeaveCriticalSection(&cs);

    printf("Result:\n");
    for (int i = 0; i < globalCount; i++)
    {
        printf("%d ", globalArray[i]);
    }
    printf("\n");

    TlsFree(dwTlsIndex);

    free(globalArray);
    free(threads);
    DeleteCriticalSection(&cs);

    return 0;
}

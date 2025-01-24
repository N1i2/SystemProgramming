#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

CRITICAL_SECTION cs;
int* globalArray = NULL;
int globalCount = 0;

__declspec(thread) int tlsStorage[10000];
__declspec(thread) int tlsCount = 0;

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

    tlsCount = 0;

    for (int i = start; i <= end; ++i)
    {
        if (isPrime(i))
        {
            tlsStorage[tlsCount++] = i;
        }
    }

    //Sleep(10000);

    EnterCriticalSection(&cs);
    for (int i = 0; i < tlsCount; ++i)
    {
        globalArray[globalCount++] = tlsStorage[i];
    }
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

    free(globalArray);
    free(threads);
    DeleteCriticalSection(&cs);

    return 0;
}

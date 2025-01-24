#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

pthread_mutex_t mutex;
pthread_once_t initOnce = PTHREAD_ONCE_INIT;

int *globalArray = NULL;
int globalCount = 0;

__thread int tlsStorage[10000];
__thread int tlsCount = 0;

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

void errorExit(const char* message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void initializeOnce() {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        errorExit("Failed to initialize mutex");
    }
}

void* L4Thread(void* param)
{
    int *range = (int *)param;
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

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < tlsCount; ++i)
    {
        globalArray[globalCount++] = tlsStorage[i];
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int compareInt(const void *a, const void *b) {
    int intA = *(int *)a;
    int intB = *(int *)b;
    return (intA - intB);
}

int main(int argc, char *argv[])
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

    pthread_once(&initOnce, initializeOnce);

    globalArray = (int *)malloc((maxBound - minBound + 1) * sizeof(int));
    if (globalArray == NULL)
    {
        printf("Error: Failed to allocate memory for globalPrimes\n");
        return 1;
    }

    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    if (threads == NULL)
    {
        perror("Error allocating threads");
        free(globalArray);
        return 1;
    }

    int rangeSize = (maxBound - minBound + 1) / numThreads;
    int remainder = (maxBound - minBound + 1) % numThreads;

    for (int i = 0; i < numThreads; i++)
    {
        int local_min = minBound + i * rangeSize;
        int local_max = local_min + rangeSize - 1;

        if (i == numThreads - 1)
        {
            local_max += remainder;
        }

        int *threadRanges = (int *)malloc(2 * sizeof(int));
        threadRanges[0] = local_min;
        threadRanges[1] = local_max;

        if (pthread_create(&threads[i], NULL, L4Thread, threadRanges) != 0)
        {
            printf("Error: Failed to create thread\n");
            free(threadRanges);
            free(globalArray);
            free(threads);
            return 1;
        }
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_lock(&mutex);
    qsort(globalArray, globalCount, sizeof(int), compareInt);
    pthread_mutex_unlock(&mutex);

    printf("Result:\n");
    for (int i = 0; i < globalCount; i++)
    {
        printf("%d ", globalArray[i]);
    }
    printf("\n");

    pthread_mutex_destroy(&mutex);
    free(globalArray);
    free(threads);
    return 0;
}

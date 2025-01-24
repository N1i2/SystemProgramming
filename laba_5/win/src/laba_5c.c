#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define START_HEAP_SIZE (1 * 1024 * 1024) // Начальный размер кучи:         1 МБ
#define MAX_HEAP_SIZE (8 * 1024 * 1024)   // Максимальный размер кучи:      8 МБ
#define BLOCK_SIZE (512 * 1024)           // Размер одного блока памяти:    512 КБ
#define INT_SIZE sizeof(int)              // Размер типа int в байтах

void HeapInfo(HANDLE heap) {
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = NULL;

    printf("Heap Information:\n");
    while (HeapWalk(heap, &entry)) {
        printf("Address: %p\tSize: %zu bytes\t", entry.lpData, (size_t)entry.cbData);
        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
            printf("Type: Allocated\n");
        else
            printf("Type: Free\n");
    }
    if (GetLastError() != ERROR_NO_MORE_ITEMS) {
        fprintf(stderr, "Error during HeapWalk.\n");
    }
    printf("\n");
}

int main() {
    HANDLE heap = HeapCreate(0, START_HEAP_SIZE, MAX_HEAP_SIZE);
    if (heap == NULL) {
        fprintf(stderr, "Failed to create heap.\n");
        return 1;
    }

    printf("Heap created.\n");
    HeapInfo(heap);
    system("pause");

    void* blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i] = HeapAlloc(heap, HEAP_ZERO_MEMORY, BLOCK_SIZE);
        if (blocks[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory block %d.\n", i);
            return 1;
        }

        int* intArray = (int*)blocks[i];
        int arraySize = BLOCK_SIZE / INT_SIZE;
        for (int j = 0; j < arraySize; j++) {
            intArray[j] = j;
        }

        printf("Allocated block %d of size 512 KB.\n", i + 1);
        HeapInfo(heap);
        system("pause & cls");
    }

    for (int i = 0; i < 10; i++) {
        if (!HeapFree(heap, 0, blocks[i])) {
            fprintf(stderr, "Failed to free memory block %d.\n", i);
        }
        else {
            printf("Freed block %d.\n", i + 1);
        }
        HeapInfo(heap);
        system("pause & cls");
    }

    if (!HeapDestroy(heap)) {
        fprintf(stderr, "Failed to destroy heap.\n");
        return 1;
    }

    printf("Heap destroyed.\n");
    return 0;
}

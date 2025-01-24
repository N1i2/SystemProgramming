#include <windows.h>
#include <stdio.h>
#include <stdint.h>

int main() {
    printf("Press Enter to continue...\n");
    getchar();

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo); 

    size_t pageSize = sysInfo.dwPageSize; 
    size_t totalPages = 256;
    size_t totalMemory = totalPages * pageSize;

    printf("Page size: %llu bytes\n", (unsigned long long)pageSize);

    // Резервирование 256 страниц виртуальной памяти
    void* reservedMemory = VirtualAlloc(NULL, totalMemory, MEM_RESERVE, PAGE_NOACCESS);
    if (!reservedMemory) {
        printf("Failed to reserve memory. Error: %lu\n", GetLastError());
        return 1;
    }
    printf("Reserved 256 pages of virtual memory at address: %p\n", reservedMemory);
    printf("Press Enter to continue...\n");
    getchar();

    // Выделение физической памяти для второй половины
    void* physicalMemory = VirtualAlloc((BYTE*)reservedMemory + (totalMemory / 2),
                                        totalMemory / 2, MEM_COMMIT, PAGE_READWRITE);
    if (!physicalMemory) {
        printf("Failed to commit memory. Error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE); 
        return 1;
    }
    printf("Committed physical memory for second half at address: %p\n", physicalMemory);
    printf("Press Enter to continue...\n");
    getchar();

    // Заполнение второй половины памяти последовательностью целых чисел
    int* intArray = (int*)physicalMemory;
    size_t numIntegers = (totalMemory / 2) / sizeof(int); 
    for (size_t i = 0; i < numIntegers; ++i) {
        intArray[i] = (int)i;
    }
    printf("Filled second half of memory with integers starting from 0.\n");
    printf("Press Enter to continue...\n");
    getchar();

    // Изменение атрибутов защиты на «только для чтения»
    DWORD oldProtect;
    if (!VirtualProtect(physicalMemory, totalMemory / 2, PAGE_READONLY, &oldProtect)) {
        printf("Failed to change memory protection. Error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return 1;
    }
    printf("Changed memory protection for second half to read-only.\n");
    printf("Press Enter to continue...\n");
    getchar();

    // Освобождение физической памяти для второй половины
    if (!VirtualFree(physicalMemory, totalMemory / 2, MEM_DECOMMIT)) {
        printf("Failed to decommit memory. Error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return 1;
    }
    printf("Decommitted physical memory for second half.\n");
    printf("Press Enter to continue...\n");
    getchar();

    // Освобождение всей зарезервированной памяти
    if (!VirtualFree(reservedMemory, 0, MEM_RELEASE)) {
        printf("Failed to release memory. Error: %lu\n", GetLastError());
        return 1;
    }
    printf("Released all reserved memory.\n");

    return 0;
}

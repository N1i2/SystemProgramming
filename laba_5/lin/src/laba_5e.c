#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    long pageSize = sysconf(_SC_PAGESIZE); 
    int numPages = 256; 
    int halfPages = numPages / 2; 
    size_t allocSize = numPages * pageSize; 

    printf("PID: %d\n", getpid()); 
    printf("Page size: %ld bytes\n", pageSize); 

    void* baseAddress = mmap(NULL, allocSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (baseAddress == MAP_FAILED) {
        perror("Error allocating memory");
        return 1;
    }
    printf("\n256 pages Base address: %p\n", baseAddress);
    getchar(); 

    if (mprotect(baseAddress, halfPages * pageSize, PROT_READ | PROT_WRITE) == -1) {
        perror("Error committing memory");
        munmap(baseAddress, allocSize);
        return 1;
    }
    printf("\nMemory committed for half: %p\n", baseAddress);
    getchar(); 

    int* intArray = (int*)baseAddress;
    for (int i = 0; i < (halfPages * pageSize) / sizeof(int); i++) {
        intArray[i] = i;
    }
    printf("\nFilled half virtual memory\n");
    getchar(); 

    if (mprotect(baseAddress, halfPages * pageSize, PROT_READ) == -1) {
        perror("Error setting memory to read-only");
        munmap(baseAddress, allocSize);
        return 1;
    }
    printf("Attribute change to read-only succeeded.\n");
    getchar(); 

    if (munmap(baseAddress, halfPages * pageSize) == -1) {
        perror("Error freeing committed memory");
        return 1;
    }
    printf("Physical memory for second half (128 pages) freed.\n");
    getchar(); 

    if (munmap(baseAddress + halfPages * pageSize, halfPages * pageSize) == -1) {
        perror("Error releasing virtual memory");
        return 1;
    }
    printf("Virtual memory (256 pages) successfully released.\n");
    getchar(); 

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK_SIZE (512 * 1024) 
#define BLOCK_COUNT 10           

int main() {
    printf("PID: %d\n", getpid()); 
    void* blocks[BLOCK_COUNT]; 

    getchar(); 

    for (int i = 0; i < BLOCK_COUNT; i++) {
        blocks[i] = malloc(BLOCK_SIZE); 
        if (blocks[i] == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        int* array = (int*)blocks[i]; 
        for (int j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            array[j] = j; 
        }
        printf("Block %d allocated at address: %p\n", i, blocks[i]);
    }

    printf("Memory allocated. Press Enter to continue...");
    getchar(); 

    for (int i = 0; i < BLOCK_COUNT; i++) {
        free(blocks[i]); 
    }

    printf("Memory freed. Press Enter to exit...");
    getchar(); 

    return 0;
}
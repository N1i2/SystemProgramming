#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int isNumb(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[]) {
   int iterations = 0;

   if (argc < 2) {
       char* env_iter_num = getenv("ITER_NUM");
       if (env_iter_num == NULL) {
           printf("Error: environment variable ITER_NUM is not set.\n");
           exit(1); 
       }
       else {
           if (isNumb(env_iter_num) == 0) {
               printf("Error: ITER_NUM is not a number.\n");
               exit(2); 
           }
           iterations = atoi(env_iter_num);
       }
   }
   else {
       if (isNumb(argv[1]) == 0) {
           printf("Error: argument is not a number.\n");
           exit(3); 
       }
       iterations = atoi(argv[1]);
   }

   printf("Count of iterations: %d\n", iterations);

   pid_t pid; 

   for (int i = 0; i < iterations; i++) {
       pid = getpid(); 
       printf("%d) PID: %d\n", i + 1, pid);
       usleep(500000); 
   }

   return 0;
}

// int main(int argc, char* argv[]) {
//     int iterations = 0;

//     if (argc < 2 || isNumb(argv[1]) == 0) {
//         printf("Error: argument is not a number or missing.\n");
//         exit(3);
//     }

//     iterations = atoi(argv[1]); 

//     printf("Count of iterations: %d\n", iterations);

//     pid_t pid; 

//     for (int i = 0; i < iterations; i++) {
//         pid = getpid();
//         printf("%d) PID: %d\n", i + 1, pid);
//         usleep(500000); 
//     }

//     return 0;
// }

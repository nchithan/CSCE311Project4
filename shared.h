#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define errExit(msg)    do{perror(msg); exit(EXIT_FAILURE); \
                        } while(0)

#define BUF_SIZE 2048   /*MAX SIZE of Buffer*/

struct shmbuf {
    sem_t sem1;
    sem_t sem2;
    sem_t sem3;
    size_t cnt;
    char buf[BUF_SIZE];
};
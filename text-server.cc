#include <sys/socket.h> // Unix header for sockets, using socket
#include <sys/un.h>     // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>     // Unix standard header, using close

#include <cassert> // using assert
#include <cerrno>  // using errno
#include <cstddef> // using size_t
#include <cstdlib> // exit
#include <cstring> // using strncpy, strerror

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <sys/sysinfo.h>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <thread>
#include "shared.h"

std::string shmpath1 = "/myshm";
char *shmpath = &shmpath1[0];

std::vector<std::string> tp;

int main(int argc, char *argv[])
{
    // if (argc != 1){
    //   return 1;
    // }
    std::cout << "SERVER START" << std::endl; // Server is starting

    sleep(5000);

    // Receiving filename and path using shared memory
    // Open Shared Memory
    int fd = shm_open(shmpath, O_RDWR, 0);
    sleep(10);
    struct shmbuf *shmp = (shmbuf *)mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fd == -1)
    {
        errExit("shm_open");
    }
    // Receiving filename and path from client through shared mem
    sleep(10);
    // Waiting for sem1
    if (sem_wait(&shmp->sem1) == -1)
    {
        errExit("sem_wait");
    }
    sleep(10);

    // std::string searchString;
    int len = 0;

    std::clog << "CLIENT REQUEST RECEIVED" << std::endl;

    std::string filepath((const char *)&shmp->buf);
    // Opening file
    std::clog << "\tOPENING: " << filepath << std::endl;

    // Uses the file descriptor to map the file to shared memory
    std::clog << "\tFILE MAPPED TO SHARED MEMORY" << std::endl;

    /* Copy data into the shared memory object. */
    std::ifstream file;
    // Opening file
    std::clog << "\tOPENING: " << filepath << std::endl;
    file.open(filepath);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            // if(line.find(searchString) != std::string::npos){
            char *charline = &line[0];
            len = strlen(charline);
            shmp->cnt = len;
            memcpy(&shmp->buf, charline, len);
            sleep(10);
            /* Tell peer that it can now access shared memory. */
            if (sem_post(&shmp->sem1) == -1)
                errExit("sem_post");
            sleep(10);
            /* Wait until peer says that it has finished accessing
            the shared memory. */
            if (sem_wait(&shmp->sem2) == -1)
            {
                errExit("sem_wait");
            }
            sleep(10);
            //}
        }
        // Adding a way to let client know when its over
        sleep(10);
        std::string quit1 = "END";
        char *quit = &quit1[0];
        len = strlen(quit);
        shmp->cnt = len;
        memcpy(&shmp->buf, quit, len);
        sleep(10);

        /* Tell peer that it can now access shared memory. */
        if (sem_post(&shmp->sem1) == -1)
            errExit("sem_post");
        sleep(10);
        std::clog << "\tFILE CLOSED" << std::endl;
    }
    else
    {
        sleep(10);
        const char *NOTVALID = "INVALID FILE";
        len = strlen(NOTVALID);
        shmp->cnt = len;
        memcpy(&shmp->buf, NOTVALID, len);
        sleep(10);
    }
    shm_unlink(shmpath);

    return 0;
}
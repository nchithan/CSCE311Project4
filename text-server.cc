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
    std::clog << "CLIENT REQUEST RECEIVED" << std::endl;

    std::string filepath((const char *)&shmp->buf);
    // Opening file
    std::clog << "\tOPENING: " << filepath << std::endl;

    //Uses the file descriptor to map the file to shared memory
    std::clog << "\tFILE MAPPED TO SHARED MEMORY" << std::endl;

    std::clog << "\tFILE CLOSED" << std::endl;

    return 0;
}
#include <sys/socket.h> // Unix header for sockets, using socket
#include <sys/un.h>     // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>     // Unix standard header, using close

#include <cassert> // using assert
#include <cerrno>  // using errno
#include <cstddef> // using size_t
#include <cstdlib> // exit
#include <cstring> // using strncpy, strerror
#include<bits/stdc++.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>
#include "shared.h"

std::vector<std::string> storage;
std::vector<std::string> storage2;
int whereItStops = 0;
std::string INVAL = "INVALID FILE";

void MakeUpper(int threadNum, int whereItStop)
{
    int size = storage.size();
    int i = whereItStop;
    for (; i < ((size / 4) * threadNum); i++)
    {
        if (storage.at(i) == "INVALID")
        {
            storage2.push_back(storage.at(i));
        } else {
            std::transform(storage.at(i).begin(), storage.at(i).end(), storage.at(i).begin(), ::toupper);
            storage2.push_back(storage.at(i));
        }
    }

    whereItStops = i;
}

int main(int argc, char *argv[])
{
    // if (argc != 3)
    //     return 1;

    int len = 0;

    std::string filePath = argv[1];
    std::string shmpath1 = "/myshm";
    char *shmpath = &shmpath1[0];

    // Use shared memory to pass the filename and path to server
    // Creating Shared mem and setting its size
    int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    sleep(10);
    if (fd == -1)
    {
        errExit("shm_open");
    }
    if (ftruncate(fd, sizeof(struct shmbuf)) == -1)
    {
        errExit("ftruncate");
    }

    /* Map the object into the caller's address space. */
    struct shmbuf *shmp = (shmbuf *)mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shmp == MAP_FAILED)
    {
        errExit("mmap");
    }
    // Initialize semaphores

    if (sem_init(&shmp->sem1, 1, 0) == -1)
    {
        errExit("sem_init-sem1");
    }
    if (sem_init(&shmp->sem2, 1, 0) == -1)
    {
        errExit("sem_init-sem2");
    }
    sleep(10);
    // Sending filePath to server
    char *charline = &filePath[0];
    len = strlen(charline);
    shmp->cnt = len;
    memcpy(&shmp->buf, charline, len);
    sleep(10);
    /* Tell peer that it can now access shared memory. */
    if (sem_post(&shmp->sem1) == -1)
        errExit("sem_post");
    sleep(10);

    std::string quit("END");
    while (true)
    {
        // Waiting for sem1
        if (sem_wait(&shmp->sem1) == -1)
        {
            errExit("sem_wait");
        }
        sleep(10);
        std::string line((const char *)&shmp->buf);
        if (line == quit)
        {
            break;
        }
        storage.push_back(line);
        sleep(10);
        if (sem_post(&shmp->sem2) == -1)
        {
            errExit("sem_post");
        }
        sleep(10);
    }

    // Creates 4 threads that process 1/4 characters of the file-backed shared memory replacing lowercase with uppercase
    int threadNum = 1;
    std::thread th1(MakeUpper, threadNum, whereItStops);
    threadNum++;
    std::thread th2(MakeUpper, threadNum, whereItStops);
    threadNum++;
    std::thread th3(MakeUpper, threadNum, whereItStops);
    threadNum++;
    std::thread th4(MakeUpper, threadNum, whereItStops);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    // Printing the results of the threads
    for (int i = 0; i < (int)storage2.size(); i++)
    {
        if (storage2.at(i) == "INVALID FILE")
        {
            std::cerr << "INVALID FILE" << std::endl;
            break;
        }
        std::cout << (i + 1) << "\t " << storage2.at(i) << std::endl;
    }

    /* Unlink the shared memory object. Even if the peer process
    is still using the object, this is okay. The object will
    be removed only after all open references are closed. */

    shm_unlink(shmpath);

    // terminates by return 0
    return 0;
}
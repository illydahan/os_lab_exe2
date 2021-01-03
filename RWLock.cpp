#include "RWLock.h"
#include <iostream>

RWLock::RWLock()
{   
    sem_init(&resource, 0, 1);
    sem_init(&rmutex, 0 , 1);
    sem_init(&serviceQueue, 0 , 1);
    readersCount = 0;
}

RWLock::~RWLock()
{
    sem_destroy(&resource);
    sem_destroy(&rmutex);
    sem_destroy(&serviceQueue);
}
// read single element
void RWLock::enterRead()
{
    sem_wait(&serviceQueue);
    sem_wait(&rmutex);
    readersCount++;
    if (readersCount == 1)
        sem_wait(&resource);

    sem_post(&serviceQueue);
    sem_post(&rmutex);

}

void RWLock::leaveRead() 
{
    sem_wait(&rmutex);     
    readersCount--;
    if(readersCount == 0)
        sem_post(&resource);

    sem_post(&rmutex);
}

// write single element
void RWLock::enterWrite()
{
    sem_wait(&serviceQueue);
    sem_wait(&resource);
    sem_post(&serviceQueue);
}

void RWLock::leaveWrite()
{
    sem_post(&resource);
}

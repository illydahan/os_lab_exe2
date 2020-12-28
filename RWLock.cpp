#include "RWLock.h"


RWLock::RWLock()
{
    readerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    writerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(readerLock, NULL);
    pthread_mutex_init(writerLock, NULL);
}

RWLock::~RWLock()
{
    pthread_mutex_destroy(readerLock);
    pthread_mutex_destroy(writerLock);
    free(readerLock);
    free(writerLock);
}

void RWLock::lockRead()
{
    pthread_mutex_lock(readerLock);
}

void RWLock::unlockRead() 
{
    pthread_mutex_unlock(readerLock);
}


void RWLock::lockWrite()
{
    pthread_mutex_lock(writerLock);
}

void RWLock::unlockWrite()
{
    pthread_mutex_unlock(writerLock);
}
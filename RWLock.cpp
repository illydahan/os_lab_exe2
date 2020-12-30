#include "RWLock.h"


RWLock::RWLock()
{
    this->readerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    this->writerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(this->readerLock, NULL);
    pthread_mutex_init(this->writerLock, NULL);
}

RWLock::~RWLock()
{
    pthread_mutex_destroy(this->readerLock);
    pthread_mutex_destroy(this->writerLock);
    free(this->readerLock);
    free(this->writerLock);
}

void RWLock::lockRead()
{
    pthread_mutex_lock(this->readerLock);
}

void RWLock::unlockRead() 
{
    pthread_mutex_unlock(this->readerLock);
}


void RWLock::lockWrite()
{
    pthread_mutex_lock(this->writerLock);
}

void RWLock::unlockWrite()
{
    pthread_mutex_unlock(this->writerLock);
}
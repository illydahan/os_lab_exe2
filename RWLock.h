#include <stdlib.h>
#include <pthread.h>


class RWLock {

private:
    pthread_mutex_t *readerLock;
    pthread_mutex_t *writerLock;

public:

    RWLock();
    ~RWLock();
    
    void initLocks();
    // reader locks operations
    void lockRead();
    void unlockRead();

    // writers locks operations
    void lockWrite();
    void unlockWrite();
    
    unsigned int readerCounter;
    // should no use this 
    unsigned int writersCounter;

};
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>

// monitor class for Reader-Writer lock ADT.
// should be be singelton (i.e one instance per process).
class RWLock {

private:
    sem_t resource;
    sem_t rmutex;
    sem_t serviceQueue;
    int readersCount;

public:

    RWLock();
    ~RWLock();
    
    void initLocks();
    // reader locks operations
    void enterRead();
    void leaveRead();

    // writers locks operations
    void enterWrite();
    void leaveWrite();

};
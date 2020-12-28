#include <stdlib.h>
#include <fstream>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "RWLock.h"
#define PATH_MAX 512


#define SUCCESS           0
#define ACCOUNT_EXISTS    1
#define WRONG_PASSWORD    2
#define WITHDREW_OVERFLOW 3
#define BUFFER_SIZE       512

class Logger {

private:
    std::string logFilePath;
    std::fstream logFileStream;
    pthread_mutex_t *loggerLock;
    char logBuffer[BUFFER_SIZE];

public:
    // create log file and initilize stream
    Logger(std::string logFilePath);
    ~Logger();

    // Methods that will log varius operations that happend during runtime.
    void logAccountOpen(int status, void *parms);
    void logDeposit(int status, void *parms);
    void logWithdrew(int status, void *parms);
    void logBalance(int status, void *parms);
    void logAccountClose(int status, void *parms);
    void logTransfer(int status, void *parms);
    void accountDosentExists(void *parms);
};


typedef struct account {
	int ID;
	int password;
	int remainer;
} account;

typedef struct threadArgs {
	std::vector<account> *myAccounts;
	account *theAccount;
	account *targetAccount;
	RWLock *dataLock;
	pthread_mutex_t *logLock;
	Logger *logObj;
	int amount;
	int password;
	int ID;
	int targetID;
	int atmID;
} threadArgs;
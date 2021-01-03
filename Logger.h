#include <stdlib.h>
#include <fstream>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include "RWLock.h"
#define PATH_MAX 512


#define SUCCESS           0
#define ACCOUNT_EXISTS    1
#define WRONG_PASSWORD    2
#define WITHDREW_OVERFLOW 3

class Logger {

private:
    std::string logFilePath;
    std::fstream logFileStream;
    pthread_mutex_t *loggerLock;

public:
    // create log file and initilize stream
    Logger();
    ~Logger();

    // Methods that will log varius operations that happend during runtime.
    void logAccountOpen(int status, void *parms);
    void logDeposit(int status, void *parms);
    void logWithdrew(int status, void *parms);
    void logBalance(int status, void *parms);
    void logAccountClose(int status, void *parms);
    void logTransfer(int status, void *parms);
    void accountDosentExists(void *parms);
    void accountWrongPassword(void *parms);
    void logBankOperation(int tax_percent, int amount, int accountID);
	static void padPassword(int password, char padded_pass []);
};


typedef struct account {
	int ID;
	int password;
	int remainer;
	RWLock * accountLock;
} account;

typedef struct threadArgs {
	std::vector<account> *myAccounts;
	account *theAccount;
	account *targetAccount;
	pthread_mutex_t *logLock;
	Logger *logObj;
	RWLock *globalLock; // will lock all the accounts
	int amount;
	int password;
	int ID;
	int targetID;
	int targetAmount;
	int atmID;
} threadArgs;

typedef struct BankOpArgs {
	std::vector<account> *accounts_vec;
	uint *bankAccount;
	RWLock *globalLock;
	Logger *logObj;
} BankOpArgs;

typedef struct snapshotArgs{
	std::vector<account> *accounts_vec;
	RWLock *globalLock;
	uint *bankBalance;
} snapshotArgs;
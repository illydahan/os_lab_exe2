#include "Logger.h"


// create log file and initilize stream
Logger::Logger()
{
    // get the current working directory
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);

    // allocate logger mutex
    loggerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    // init log mutex
    pthread_mutex_init(loggerLock, NULL);
    // define log file path
    std::string log_file_path = std::string(cwd) + std::string("/log.txt");
    // open or create if dosent exists, log file.
    logFileStream.open(log_file_path, std::fstream::out);

}

Logger::~Logger()
{
    logFileStream.close();
}

void Logger::accountDosentExists(void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    
    logFileStream << "Error " << argsPtr->atmID << ": Your transaction failed – account id " << argsPtr->ID;
    logFileStream << " does not exist\n";
}

void Logger::accountWrongPassword(void *parms) 
{
    threadArgs *argsPtr = (threadArgs *)parms;
    logFileStream << "Error " <<argsPtr->atmID << ": Your transaction failed – password for account id ";
    logFileStream << argsPtr->ID << " is incorrect\n";
}

// Methods that will log varius operations that happend during runtime.
void Logger::logAccountOpen(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case ACCOUNT_EXISTS:    
        logFileStream << "Error " << argsPtr->atmID << ": Your transaction failed – account id " << argsPtr->ID;
        logFileStream << " already exist's\n";
        break;
        
    case SUCCESS:
        logFileStream << argsPtr->atmID << " : New account id is " << argsPtr->ID;
        logFileStream << " with password " << argsPtr->password << " and initial balance ";
        logFileStream << argsPtr->amount << std::endl;
        break;
    
    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);

}

void Logger::logDeposit(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case WRONG_PASSWORD:
        accountWrongPassword(parms);
        break;
    case SUCCESS:
        logFileStream << argsPtr->atmID << " : Account ";
        logFileStream <<  argsPtr->ID<<" new balance is " << argsPtr->targetAmount;
        logFileStream << " after " << argsPtr->amount << " $ was deposited\n";
        break;
    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}

void Logger::logWithdrew(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case WRONG_PASSWORD:
        accountWrongPassword(parms);
        break;

    case WITHDREW_OVERFLOW:
        logFileStream << "Error " << argsPtr->atmID <<": Your transaction failed ";
        logFileStream <<"– account id " <<argsPtr->ID << " balance is lower than ";
        logFileStream <<argsPtr->amount << "\n";
        break;

    case SUCCESS:
        logFileStream << argsPtr->atmID << " : Account ";
        logFileStream <<  argsPtr->ID<<" new balance is " << argsPtr->targetAmount;
        logFileStream << " after " << argsPtr->amount << " $ was withdrew\n";
        break;

    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}

void Logger::logBalance(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case WRONG_PASSWORD:
        accountWrongPassword(parms);
        break;

    case SUCCESS:
        logFileStream << argsPtr->atmID <<" : Account "<< argsPtr->ID;
        logFileStream << " balance is " << argsPtr->amount << "\n";
        break;

    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}

void Logger::logAccountClose(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case WRONG_PASSWORD:
        accountWrongPassword(parms);
        break;
    
    case SUCCESS:
        logFileStream << argsPtr->atmID << " : Account " << argsPtr->ID << " is now closed. Balance was ";
        logFileStream << argsPtr->amount << " \n";
        break;
    
    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}

void Logger::logTransfer(int status, void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    pthread_mutex_lock(loggerLock);
    std::string logMsg;
    switch (status)
    {
    case WRONG_PASSWORD:
        accountWrongPassword(parms);
        break;
    
    case WITHDREW_OVERFLOW:
        logFileStream <<"Error " << argsPtr->atmID<< " : Your transaction failed – account id ";
        logFileStream << argsPtr->ID <<" balance is lower than " << argsPtr->amount << " \n";
        break;
    
    case SUCCESS:
        logFileStream <<argsPtr->atmID <<" : Transfer "<< argsPtr->amount;
        logFileStream << " from account " << argsPtr->theAccount->ID <<" to account ";
        logFileStream << argsPtr->theAccount->ID << " new account balance is ";
        logFileStream << argsPtr->theAccount->remainer << " new target account balance is  ";
        logFileStream << argsPtr->targetAccount->remainer << " \n"; 
        break;
    
    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}

void Logger::logBankOperation(int tax_percent, int amount, int accountID) 
{
    pthread_mutex_lock(loggerLock);

    logFileStream << "Bank: commision of " << tax_percent << "%";
    logFileStream << " were charged, the bank gained ";
    logFileStream << amount << " $" << " from account " << accountID;
    logFileStream << "\n"; 
    pthread_mutex_unlock(loggerLock);
}

// we assume password length of 4;
// todo: check
void Logger::padPassword(int password, char padded_pass []) 
{
	if(	password / 10 == 0) {
		sprintf(padded_pass, "000%d", password); 
	}
	else if (password / 100 == 0) {
		sprintf(padded_pass, "00%d", password);
	}
	else if(password / 1000 == 0) {
		sprintf(padded_pass, "0%d", password);
	}
    else
    {
        sprintf(padded_pass, "%d", password);
    }
}
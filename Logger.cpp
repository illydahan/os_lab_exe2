#include "Logger.h"


// create log file and initilize stream
Logger::Logger(std::string logFilePath)
{
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    loggerLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(loggerLock, NULL);
    std::string log_file_path = std::string(cwd) + std::string("/log.txt");
    logFileStream.open(log_file_path,std::fstream::app | std::fstream::out);
    memset(logBuffer, '\0', BUFFER_SIZE);
}

Logger::~Logger()
{
    //delete[] logFileStream;
    //delete[] logFilePath;
    //delete[] mutex
    free(loggerLock);
    free(logBuffer);
    logFileStream.close();
}

void Logger::accountDosentExists(void *parms)
{
    threadArgs *argsPtr = (threadArgs *)parms;
    std::string logMsg = "Error %d: Your transaction failed – account id %d does not exist\n";
    snprintf(logBuffer, 
            BUFFER_SIZE, 
            logMsg.c_str(), 
            argsPtr->atmID,
            argsPtr->targetAccount->ID);

    logFileStream.write(logBuffer,logMsg.length());
    logFileStream.flush();
    memset(logBuffer, '\0', BUFFER_SIZE);
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
        logMsg = "Error %d: Your transaction failed – can't create account with negative balance\n";
        snprintf(logBuffer, 
                logMsg.length(), 
                logMsg.c_str(), 
                argsPtr->atmID);

        logFileStream.write(logBuffer, logMsg.length());
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    case SUCCESS:
        snprintf(logBuffer,
                BUFFER_SIZE,
                "%d: New account id is %d with password %d and initial balance %d\n",
                argsPtr->atmID,
                argsPtr->targetAccount->ID,
                argsPtr->targetAccount->password,
                argsPtr->targetAccount->remainer);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
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

    switch (status)
    {
    case WRONG_PASSWORD:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed –password for account id %d is incorrect\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    case SUCCESS:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "%d: Account %d new balance is %d after %d $ was deposited\n", 
                argsPtr->atmID,
                argsPtr->targetAccount->ID,
                argsPtr->targetAccount->remainer,
                argsPtr->amount);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
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

    switch (status)
    {
    case WRONG_PASSWORD:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed – password for account id %d is incorrect\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;

    case WITHDREW_OVERFLOW:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed – account id %d balance is lower than %d\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID,
                argsPtr->amount);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;

    case SUCCESS:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "%d: Account %d new balance is %d after %d$ was withdrew\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID,
                argsPtr->theAccount->remainer,
                argsPtr->amount);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
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

    switch (status)
    {
    case WRONG_PASSWORD:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed –password for account id %d is incorrect\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;

    case SUCCESS:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "%d: Account %d balance is %d\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID,
                argsPtr->theAccount->remainer);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
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

    switch (status)
    {
    case WRONG_PASSWORD:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed –password for account id %d is incorrect\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    
    case SUCCESS:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "%d: Account %d is now closed. Balance was %d\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID,
                argsPtr->theAccount->remainer);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
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

    switch (status)
    {
    case WRONG_PASSWORD:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed –password for account id %d is incorrect\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    
    case WITHDREW_OVERFLOW:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "Error %d: Your transaction failed – account id %d balance is lower than %d\n", 
                argsPtr->atmID,
                argsPtr->theAccount->ID,
                argsPtr->amount);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    
    case SUCCESS:
        snprintf(logBuffer, 
                BUFFER_SIZE, 
                "%d: Transfer %d from account %d to account " \
                "%d new account balance is %d " \
                "new target account balance is %d\n", 
                argsPtr->atmID,
                argsPtr->amount,
                argsPtr->theAccount->ID,
                argsPtr->targetAccount->ID,
                argsPtr->theAccount->remainer,
                argsPtr->targetAccount->remainer);

        logFileStream.write(logBuffer, sizeof(logBuffer));
        logFileStream.flush();
        memset(logBuffer, '\0', BUFFER_SIZE);
        break;
    
    default:
        break;
    }
    pthread_mutex_unlock(loggerLock);
}



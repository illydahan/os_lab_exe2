//		ATM .cpp
//********************************************
#include "ATM.h"
#define longest_cmd 300

ATM::ATM(std::string commandsFilePath)
{
	// open read mode
	

	std::string line;
	std::ifstream commandFile(commandsFilePath);
}

ATM::~ATM()
{
	// TODO
}

void ATM::readLine(std::string& command, std::fstream& commandFile)
{
	std::getline(commandFile, command);
}

void * ATM::openAccout(void *args)
{
	// prologue
	threadArgs *argsPtr = (threadArgs *)args;
	Logger *log  = argsPtr->logObj;
	RWLock *dataLock = argsPtr->dataLock;
	pthread_mutex_t *logLock = argsPtr->logLock; 

	// Here we will check for existing account with the same ID.

	dataLock->lockRead();
	for (account index : argsPtr->myAccounts)
	{
		if (argsPtr->targetAccount.ID == index.ID)	{
			pthread_mutex_lock(logLock);
			log->logAccountOpen(ACCOUNT_EXISTS, args);
			pthread_mutex_unlock(logLock);
			dataLock->unlockRead();
			return (void *) RET_FAILURE;
		}
	}
	dataLock->unlockRead();

	if (argsPtr->targetAccount.remainer < 0) {
		pthread_mutex_lock(logLock);
		log->logAccountOpen(WITHDREW_OVERFLOW, args);
		pthread_mutex_unlock(logLock);
		return (void *) RET_FAILURE;
	}

	// add new account to the account vector buffer.
	dataLock->lockWrite();
	argsPtr->myAccounts.push_back(argsPtr->targetAccount);
	dataLock->unlockWrite();
	
	pthread_mutex_lock(logLock);
	log->logAccountOpen(SUCCESS, argsPtr);
	pthread_mutex_lock(logLock);
	return (void *) RET_SUCESS;
}



//********************************************
// function name: Deposit
// Description: Deposits amount of money in an account, if given password is correct.
// Parameters: reference to account, password of the account, the amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

void * ATM::Deposit(void *args) {

	threadArgs *argsPtr = (threadArgs *)args;
	account the_account;
	
	argsPtr->dataLock->lockRead();
	accountAvailible(argsPtr->myAccounts, the_account, argsPtr->ID);
	int check = the_account.password;
	argsPtr->dataLock->unlockRead();

	if (check != argsPtr->password) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logDeposit(WRONG_PASSWORD, argsPtr);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}

	argsPtr->dataLock->lockWrite();
	argsPtr->theAccount.remainer += argsPtr->amount;
	argsPtr->dataLock->unlockWrite();

	pthread_mutex_lock(argsPtr->logLock);
	argsPtr->logObj->logDeposit(SUCCESS, args);
	pthread_mutex_unlock(argsPtr->logLock);
	
	return (void *) RET_SUCESS;
}


//main will check if the account exists, before bothering the machine
void * ATM::Withdrew(void *args) {
	/*
	 * reader + writer
	 */
	threadArgs *argsPtr = (threadArgs *)args;
	account the_account;
	
	argsPtr->dataLock->lockRead();
	accountAvailible(argsPtr->myAccounts, the_account, argsPtr->ID);
	int check = the_account.password;
	argsPtr->dataLock->unlockRead();
	
	if (check != argsPtr->password) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logWithdrew(WRONG_PASSWORD, args);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}
	if(argsPtr->theAccount.remainer < argsPtr->amount) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logWithdrew(WITHDREW_OVERFLOW, args);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}


	argsPtr->dataLock->lockWrite();
	argsPtr->theAccount.remainer -= argsPtr->amount;
	argsPtr->dataLock->unlockWrite();
	pthread_mutex_lock(argsPtr->logLock);
	argsPtr->logObj->logWithdrew(SUCCESS, args);
	pthread_mutex_unlock(argsPtr->logLock);

	return (void *) RET_SUCESS;
}

//********************************************
// function name: Balance
// Description: shows the balance of the account, if password is correct
// Parameters: reference to account, password of the account
// Returns: 0 - success,1 - failure
//**************************************************************************************

void * ATM::Balance(void *args){

	threadArgs *argsPtr = (threadArgs *)args;
	account the_account;
	
	argsPtr->dataLock->lockRead();
	accountAvailible(argsPtr->myAccounts, the_account, argsPtr->ID);
	int check = the_account.password;
	argsPtr->dataLock->unlockRead();

	if (check != argsPtr->password) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logBalance(WRONG_PASSWORD, args);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}
	
	pthread_mutex_lock(argsPtr->logLock);
	argsPtr->logObj->logBalance(SUCCESS, args);
	pthread_mutex_unlock(argsPtr->logLock);
	return (void *) RET_SUCESS;
}


void * ATM::closeAccount(void *args) {

	/*
	 * WRITER
	 */
	threadArgs *argsPtr = (threadArgs *)args;

	account the_account;
	
	argsPtr->dataLock->lockRead();
	int index = accountAvailible(argsPtr->myAccounts, the_account, argsPtr->ID);
	int check = the_account.password;
	argsPtr->dataLock->unlockRead();

	if (check != argsPtr->password) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logAccountClose(WRONG_PASSWORD, args);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}


	argsPtr->myAccounts.erase(argsPtr->myAccounts.begin() + index);
	return (void *) RET_SUCESS;

}


//********************************************
// function name: Transfer
// Description: transfer money between accounts, if password is correct.
// Parameters: reference to account, password of the account, reference to taget acount, amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

void * ATM::Transfer(void *args){

	threadArgs *argsPtr = (threadArgs *)args;

	account the_account, target_account;
	
	argsPtr->dataLock->lockRead();
	accountAvailible(argsPtr->myAccounts, the_account, argsPtr->ID);
	accountAvailible(argsPtr->myAccounts, target_account, argsPtr->targetID);
	bool check = the_account.password != argsPtr->password ? true : false;
	argsPtr->dataLock->unlockRead();

	if (check) {
		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logTransfer(WRONG_PASSWORD, args);
		pthread_mutex_unlock(argsPtr->logLock);
		return (void *) RET_FAILURE;
	}

	argsPtr->dataLock->lockRead();
	check = argsPtr->theAccount.remainer < argsPtr->amount ? true : false;
	argsPtr->dataLock->unlockRead();
	
	if (check) {

		pthread_mutex_lock(argsPtr->logLock);
		argsPtr->logObj->logTransfer(WITHDREW_OVERFLOW, args);
		pthread_mutex_unlock(argsPtr->logLock);
		
		return (void *) RET_FAILURE;
	}

	argsPtr->dataLock->lockRead();
	argsPtr->dataLock->lockWrite();
	the_account.remainer -= argsPtr->amount;
	target_account.remainer += argsPtr->amount;
	
	pthread_mutex_lock(argsPtr->logLock);
	argsPtr->logObj->logTransfer(SUCCESS, args);
	pthread_mutex_unlock(argsPtr->logLock);
	
	argsPtr->dataLock->unlockRead();
	argsPtr->dataLock->unlockWrite();
	return (void *) RET_SUCESS;
}

int ATM::accountAvailible(std::vector<account>& account_vec, account& targetAcc, int ID) {

	unsigned long accIndex;

	for (accIndex = 0; accIndex < account_vec.size() ; accIndex++)
	{
		if(ID == account_vec.at(accIndex).ID) {
			targetAcc = account_vec.at(accIndex);
			break;
		}
	}
	return accIndex;
}
//		ATM .cpp
//********************************************
#include "ATM.h"
#define longest_cmd 300


// TODO: Handle account not in list
void ATM::readLine(std::string& command, std::fstream& commandFile)
{
	std::getline(commandFile, command);
}

void * ATM::openAccout(void *args)
{

	// prologue
	threadArgs *argsPtr = (threadArgs *)args;
	Logger *log  = argsPtr->logObj;
	
	// Here we will check for existing account with the same ID.
	argsPtr->globalLock->enterRead();
	int myAccindex = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	argsPtr->globalLock->leaveRead();
	if(myAccindex != -1) {
		argsPtr->logObj->logAccountOpen(ACCOUNT_EXISTS,args);
		return (void *) RET_FAILURE;
	}
	argsPtr->targetAccount->ID = argsPtr->ID;
	argsPtr->targetAccount->password = argsPtr->password;
	argsPtr->targetAccount->remainer = argsPtr->amount;
	
	if((*argsPtr->myAccounts).size() == 0)
	{
		argsPtr->globalLock->enterWrite();
		sleep(1);
		argsPtr->myAccounts->push_back(*argsPtr->targetAccount);
		argsPtr->globalLock->leaveWrite();
		log->logAccountOpen(SUCCESS, args);
		return (void *) RET_SUCESS;
	}

	uint index;
	for (index = 0; index < argsPtr->myAccounts->size(); index++)
	{
		if( argsPtr->ID < (argsPtr->myAccounts->at(index)).ID )
		{
			argsPtr->globalLock->enterWrite();
			sleep(1);
			argsPtr->myAccounts->emplace(argsPtr->myAccounts->begin() + index, *argsPtr->targetAccount);

			argsPtr->globalLock->leaveWrite();
			log->logAccountOpen(SUCCESS, argsPtr);
			return (void *) RET_SUCESS;
		}
	}
	argsPtr->globalLock->enterWrite();
	sleep(1);
	(*argsPtr->myAccounts).push_back(*argsPtr->targetAccount);
	argsPtr->globalLock->leaveWrite();
	log->logAccountOpen(SUCCESS, argsPtr);
	return (void *) RET_SUCESS;
}



//********************************************
// function name: Deposit
// Description: Deposits amount of money in an account, if given password is correct.
// Parameters: reference to account, password of the account, the amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

void *ATM::Deposit(void *args) {

	// prologue
	threadArgs *argsPtr = (threadArgs *)args;
	//Logger *log  = argsPtr->logObj;
	//pthread_mutex_t *logLock = argsPtr->logLock; 

	// Here we will check for existing account with the same ID.

	
	argsPtr->globalLock->enterRead();
	int index = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	argsPtr->globalLock->leaveRead();
	if(index == -1) {
		argsPtr->logObj->accountDosentExists(args);
		return (void *) RET_FAILURE;
	}

	
	bool check = argsPtr->myAccounts->at(index).password;
	
	if (check != argsPtr->password) {

		argsPtr->logObj->logDeposit(WRONG_PASSWORD, argsPtr);
		return (void *) RET_FAILURE;
	}
	

	argsPtr->globalLock->enterRead();
	argsPtr->myAccounts->at(index).accountLock->enterWrite();
	sleep(1);
	argsPtr->targetAmount = argsPtr->myAccounts->at(index).remainer;
	argsPtr->myAccounts->at(index).remainer += argsPtr->amount;
	argsPtr->amount = argsPtr->myAccounts->at(index).remainer;

	argsPtr->myAccounts->at(index).accountLock->leaveWrite();
	argsPtr->globalLock->leaveRead();
	argsPtr->logObj->logDeposit(SUCCESS, args);	
	return (void *) RET_SUCESS;
}

void * ATM::Withdrew(void *args) {

	threadArgs *argsPtr = (threadArgs *)args;	

	
	argsPtr->globalLock->enterRead();
	int index = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	argsPtr->globalLock->leaveRead();
	if(index == -1) {
		argsPtr->logObj->accountDosentExists(args);
		return (void *) RET_FAILURE;
	}
	
	argsPtr->myAccounts->at(index).accountLock->enterRead();
	int check = argsPtr->myAccounts->at(index).password;
	argsPtr->myAccounts->at(index).accountLock->leaveRead();
	// validate password
	if (check != argsPtr->password) {
		argsPtr->logObj->logWithdrew(WRONG_PASSWORD, args);
		
		return (void *) RET_FAILURE;
	}
	// check current balance larger then withdrew amount
	argsPtr->globalLock->enterRead();
	argsPtr->myAccounts->at(index).accountLock->enterRead();
	sleep(1);
	if(argsPtr->myAccounts->at(index).remainer < argsPtr->amount) {
		account the_account = argsPtr->myAccounts->at(index);
		argsPtr->theAccount = &the_account;
		argsPtr->myAccounts->at(index).accountLock->leaveRead();
		argsPtr->globalLock->leaveRead();
		argsPtr->logObj->logWithdrew(WITHDREW_OVERFLOW, args);
		
		return (void *) RET_FAILURE;
	}
	// do withdrew
	argsPtr->myAccounts->at(index).accountLock->leaveRead();
	argsPtr->myAccounts->at(index).accountLock->enterWrite();
	argsPtr->myAccounts->at(index).remainer -= argsPtr->amount;
	argsPtr->myAccounts->at(index).accountLock->leaveWrite();
	argsPtr->targetAmount = argsPtr->myAccounts->at(index).remainer;
	argsPtr->globalLock->leaveRead();
	argsPtr->logObj->logWithdrew(SUCCESS, args);

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

	argsPtr->globalLock->enterRead();
	int index = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	argsPtr->globalLock->leaveRead();
	if(index == -1) {
		argsPtr->logObj->accountDosentExists(args);
		return (void *) RET_FAILURE;
	}	

	argsPtr->myAccounts->at(index).accountLock->enterRead();
	int check = argsPtr->myAccounts->at(index).password;
	argsPtr->myAccounts->at(index).accountLock->leaveRead();
	if (check != argsPtr->password) {
		argsPtr->logObj->logBalance(WRONG_PASSWORD, args);
		return (void *) RET_FAILURE;
	}
	argsPtr->globalLock->enterRead();
	argsPtr->myAccounts->at(index).accountLock->enterRead();
	sleep(1);
	argsPtr->amount = argsPtr->myAccounts->at(index).remainer;
	argsPtr->myAccounts->at(index).accountLock->leaveRead();
	argsPtr->globalLock->leaveRead();
	argsPtr->logObj->logBalance(SUCCESS, args);
	return (void *) RET_SUCESS;
}


void * ATM::closeAccount(void *args) {


	threadArgs *argsPtr = (threadArgs *)args;	
	
	argsPtr->globalLock->enterRead();
	int index = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	argsPtr->globalLock->leaveRead();
	if(index == -1) {
		argsPtr->logObj->accountDosentExists(args);
		return (void *) RET_FAILURE;
	}

	argsPtr->myAccounts->at(index).accountLock->enterRead();
	int check = argsPtr->myAccounts->at(index).password;
	argsPtr->myAccounts->at(index).accountLock->leaveRead();

	if (check != argsPtr->password) {
		// wrong password
		argsPtr->logObj->logAccountClose(WRONG_PASSWORD, args);
		return (void *) RET_FAILURE;
	}
	
	argsPtr->myAccounts->at(index).accountLock->enterRead();
	argsPtr->amount = argsPtr->myAccounts->at(index).remainer;
	argsPtr->myAccounts->at(index).accountLock->leaveRead();

	argsPtr->globalLock->enterWrite();
	sleep(1);
	
	argsPtr->myAccounts->erase(argsPtr->myAccounts->begin() + index);
	argsPtr->globalLock->leaveWrite();
	argsPtr->logObj->logAccountClose(SUCCESS, args);
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
	
	// transfer between same accounts
	if(argsPtr->ID == argsPtr->targetID) {
		argsPtr->logObj->logTransfer(RET_FAILURE, args);
		
		return (void *) RET_FAILURE;
	}


	int sourceIndex = accountAvailible(argsPtr->myAccounts, argsPtr->ID);
	int targetIndex = accountAvailible(argsPtr->myAccounts, argsPtr->targetID);

	if(sourceIndex == -1) {
		argsPtr->logObj->accountDosentExists(argsPtr);
		return (void *) RET_FAILURE;
	}
	else if(targetIndex == -1)
	{
		// change id to log to the target id
		argsPtr->ID = argsPtr->targetID;
		argsPtr->logObj->accountDosentExists(argsPtr);
		return (void *) RET_FAILURE;
	}
	if(sourceIndex == targetIndex) {
		return (void *) RET_FAILURE;
	}

	argsPtr->globalLock->enterRead();
	bool check = argsPtr->myAccounts->at(sourceIndex).password != argsPtr->password ? true : false;
	bool check2 = argsPtr->myAccounts->at(sourceIndex).remainer < argsPtr->amount ? true : false;
	argsPtr->globalLock->leaveRead();

	// wrong password
	if (check) {
		argsPtr->logObj->logTransfer(WRONG_PASSWORD, args);
		return (void *) RET_FAILURE;
	}

	// not enough money for transaction
	else if (check2) {
		argsPtr->logObj->logTransfer(WITHDREW_OVERFLOW, args);
		return (void *) RET_FAILURE;
	}

	argsPtr->globalLock->enterRead();
	argsPtr->myAccounts->at(sourceIndex).accountLock->enterWrite();
	argsPtr->myAccounts->at(targetIndex).accountLock->enterWrite();
	
	sleep(1);
	
	argsPtr->myAccounts->at(sourceIndex).remainer -= argsPtr->amount;
	

	argsPtr->myAccounts->at(targetIndex).remainer += argsPtr->amount;
	
	account the_account = argsPtr->myAccounts->at(sourceIndex);
	account target_account = argsPtr->myAccounts->at(sourceIndex);
	argsPtr->theAccount = &the_account;
	argsPtr->targetAccount = &target_account;
	argsPtr->myAccounts->at(targetIndex).accountLock->leaveWrite();
	argsPtr->myAccounts->at(sourceIndex).accountLock->leaveWrite();
	argsPtr->globalLock->leaveRead();
	argsPtr->logObj->logTransfer(SUCCESS, args);

	
	return (void *) RET_SUCESS;
	
}

int inline ATM::accountAvailible(std::vector<account> *account_vec, int ID) {

	
	unsigned long accIndex;
	uint size = account_vec->size();

	for (accIndex = 0; accIndex < size ; accIndex++)
	{
		if(ID == account_vec->at(accIndex).ID) {
			return accIndex;
		}
	}
	
	return -1;
	
}

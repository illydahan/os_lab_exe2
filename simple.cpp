//		simple .cpp
//********************************************
#include "simple.h"


void *openAccout(void *args)
{
	/*
	 * reader + writer
	 */
	// Here we will check for existing account with the same ID.
	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int retval;

	if(initialAmount < 0) {
		std::cout << "Error <ATM ID>: Your transaction failed – can't create account with negative balance" << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}


	// initialize new account structure
	account new_account = {ID, password, initialAmount};
	// add new account to the account vector buffer.
	account_vec.push_back(new_account);
	std::cout <<< "ATM ID>: New account id is " << ID << " with password " << password <<" and initial balance " << initialAmount << std::endl;
	ret_val = 0;
	return (void *)&ret_val;
}



//********************************************
// function name: Deposit
// Description: Deposits amount of money in an account, if given password is correct.
// Parameters: reference to account, password of the account, the amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

//main will check if the account exists, before bothering the machine
void *Deposit(void *args) {
	/*
	 * reader + writer
	 */
	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int ret_val, check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed � password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}
	the_account.remainer += amount;
	std::cout << "ATM ID TO HER PLEASE" << ": Account " << the_account.ID << " new balance is ";
	std::cout << the_account.remainer << " after " << amount << " $ was deposited" << std::endl;
	ret_val = 0;
	return (void *)&ret_val;
}


//main will check if the account exists, before bothering the machine
void *Withdrew(void *args) {
	/*
	 * reader + writer
	 */
	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int ret_val;
	int check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed � password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}
	if(the_account.remainer < amount) {
		std::cout << "Error <ATM ID>: Your transaction failed – account id ";
		std::cout << the_account.ID << " balance is lower than " << amount << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}
	the_account.remainer -= amount;
	std::cout << "ATM ID TO HER PLEASE" << ": Account " << the_account.ID << " new balance is ";
	std::cout << the_account.remainer << " after " << amount << " $ was withdrew" << std::endl;
	ret_val = 0;
	return (void *)&ret_val;
}

//********************************************
// function name: Balance
// Description: shows the balance of the account, if password is correct
// Parameters: reference to account, password of the account
// Returns: 0 - success,1 - failure
//**************************************************************************************

void *Balance(void *args){

	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int ret_val, check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed � password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}
	std::cout << "ATM ID HERE" << " : Account " << the_account.ID << " balance is ";
	std::cout << the_account.remainer << std::endl;
	ret_val = 0;
	return (void *)&ret_val;
}


void *closeAccount(void *args) {

	/*
	 * WRITER
	 */
	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int ret_val;
	if(password != the_account.password) {
		std::cout << "Error <ATM ID>: Your transaction failed – password for account id";
		std::cout << " ID " << std::endl;
		ret_val = 1;
		return (void *)&ret_val;
	}

	int index = -1;
	int counter = 0;
	// find the specified account index
	for(account acc : account_vac) {
		if(acc.ID == the_account.ID) {
			index = counter;
			break;
		}
		++counter;
	}


	// remove account
	if(index != -1) {
		account_vec.erase(index);
		ret_val = 0;
		return (void *)&ret_val;
	}
	// we shouldn't reach that point
	std::cout << "error, we shouldnt be here" << std::endl;
	return &ret_val;

}


//********************************************
// function name: Transfer
// Description: transfer money between accounts, if password is correct.
// Parameters: reference to account, password of the account, reference to taget acount, amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

void *Transfer(void *args){

	threadArgs *argsPtr = (threadArgs *)args;
	int initialAmount = argsPtr->amount;
	int ID = argsPtr->ID;
	int password = argsPtr->password;
	account the_account = argsPtr->the_account;
	std::vector<account> account_vec = argsPtr->myAccounts;

	int ret_val, check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed � password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		ret_val = 1;
		return &ret_val;
	}
	check = the_account.remainer;
	if (check < amount) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed � account id ";
		std::cout << the_account.ID << " balance is lower than " << amount << std::endl;
		ret_val = 1;
		return &ret_val;
	}
	the_account.remainer -= amount;
	target_account.remainer += amount;
	std::cout << "ATM ID HERE" << ": Transfer " << amount << " from account " << the_account.ID;
	std::cout << " to account " << target_account.ID << " new account balance is " << the_account.remainer;
	std::cout << " new target account balance is " << target_account.remainer << std::endl;
	ret_val = 0;
	return &ret_val;
}


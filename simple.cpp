//		simple .cpp
//********************************************
#include "simple.h"
//********************************************
// function name: Deposit
// Description: Deposits amount of money in an account, if given password is correct.
// Parameters: reference to account, password of the account, the amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

//main will check if the account exists, before bothering the machine
int Deposit(account& the_account, int password, int amount) {
	int check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed – password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		return 1;
	}
	the_account.remainer += amount;
	std::cout << "ATM ID TO HER PLEASE" << ": Account " << the_account.ID << " new balance is ";
	std::cout << the_account.remainer << " after " << amount << " $ was deposited" << std::endl;
	return 0;
}

//********************************************
// function name: Balance
// Description: shows the balance of the account, if password is correct
// Parameters: reference to account, password of the account
// Returns: 0 - success,1 - failure
//**************************************************************************************

int Balance(account& the_account, int password){
	int check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed – password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		return 1;
	}
	std::cout << "ATM ID HERE" << " : Account " << the_account.ID << " balance is ";
	std::cout << the_account.remainer << std::endl;
	return 0;
}

//********************************************
// function name: Transfer
// Description: transfer money between accounts, if password is correct.
// Parameters: reference to account, password of the account, reference to taget acount, amount of money
// Returns: 0 - success,1 - failure
//**************************************************************************************

int Transfer(account& the_account, int password, account& target_account, int amount){
	int check = the_account.password;
	if (check != password) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed – password for account id ";
		std::cout << the_account.ID << " is incorrect" << std::endl;
		return 1;
	}
	check = the_account.remainer;
	if (check < amount) {
		std::cout << "Error " << "ATM ID TO HER PLEASE" << ": Your transaction failed – account id ";
		std::cout << the_account.ID << " balance is lower than " << amount << std::endl;
		return 1;
	}
	the_account.remainer -= amount;
	target_account.remainer += amount;
	std::cout << "ATM ID HERE" << ": Transfer " << amount << " from account " << the_account.ID;
	std::cout << " to account " << target_account.ID << " new account balance is " << the_account.remainer;
	std::cout << " new target account balance is " << target_account.remainer << std::endl;
	return 0;
}
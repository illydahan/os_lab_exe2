#ifndef _ATM_H
#define _ATM_H 
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include "Logger.h"


#define RET_SUCESS 0
#define RET_FAILURE 1




class ATM {


public:
	static void readLine(std::string& command, std::fstream& commandFile);
	static int accountAvailible(std::vector<account> *account_vec, int ID);
	// Pthread compatible function that perform varius atm operations
	static void *openAccout(void *args);
	static void *Deposit(void *args);
	static void *Withdrew(void *args);
	static void *Balance(void *args);
	static void *closeAccount(void *args);
	static void *Transfer(void *args);
};

#endif 

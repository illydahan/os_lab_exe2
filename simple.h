#ifndef _SIMPLE_H
#define _SIMPLE_H
#include <unistd.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>
#include <pthreads>

typedef struct account {
	int ID;
	int password;
	int remainer;
}account;

typedef struct threadArgs {
	std::vector<account> myAccounts;
	account theAccount;
	int password;
	int ID;
	int amount;
};

int Deposit(account&, int, int);
int Balance(account&, int);
int Transfer(account&, int, account&, int);
#endif 

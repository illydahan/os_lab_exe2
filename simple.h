#ifndef _SIMPLE_H
#define _SIMPLE_H
#include <unistd.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>

typedef struct account {
	int ID;
	int password;
	int remainer;
}account;

int Deposit(account&, int, int);
int Balance(account&, int);
int Transfer(account&, int, account&, int);
#endif 
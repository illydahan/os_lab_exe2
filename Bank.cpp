//		skeleton .cpp
//********************************************

#include <string.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include "ATM.h"

// here we will store all the threads
static std::vector<pthread_t> threadsLst = std::vector<pthread_t>();
// allocated threads-related memory(..arguments).
static std::vector<void *> allocated_mem = std::vector<void *>();


//declaring functions in the main. may be moved.
void HandleInputLine(std::string& inputLine, 
					 std::vector<std::string>& command_sep);

bool ProcessCommand(std::vector<std::fstream>& commandFiles, 
					uint currentAtmMachine,
					std::vector<std::string>& command_sep);

void executeAtmOperation(std::vector<account>& account_vec,
						std::vector<std::string>& command_sep,
						std::vector<pthread_t>& active_threads,
						Logger *logger,
						int currentAtmMachine,
						RWLock *masterLock);


// thread redy bank operation function
void *BankOperations(void *args);

// will print to stdio account snapshot
void *printAccountSnapshot(void *args);

int main(int argc, char *argv[]) {

	auto start = std::chrono::system_clock::now();

	// validate at least 3 arguments given
	if(argc < 3) {
		std::cout << "usage: ./Bank N file1 file2.." << std::endl;
		return -1;
	}
	// validate number of atm's
	uint atm_number = atoi(argv[1]);
	if(atm_number < 1) {
		std::cout << "at least one atm is required" << std::endl;
		return -1;
	}

	// Parsed input command
	std::vector<std::string> command_sep = std::vector<std::string>();
	
	// Here we'll store all the accounts and there locks
	std::vector<account> account_vec = std::vector<account>();

	// A vector that stores all the command files
	std::vector<std::fstream> commandFiles = std::vector<std::fstream>();

	// An object that will perform logging operations
	Logger *logger = new Logger();


	RWLock *globalLock = new RWLock();

	// bank master account balance
	static uint bankBalance = 0;


	// all the possible commands
	std::string possible_commands[] = {std::string("O"), 
										std::string("D"), 
										std::string("W"), 
										std::string("B") ,
										std::string("Q") ,
										std::string("T")};

	// initialize all the atm objects and add them to the vector.
	for(uint atm_index = 0; atm_index < atm_number; atm_index++)
	{	
		// first argument is the prorgam name , seconed is the number of the atm's so we'll skip those.
		std::string commandFilePath(argv[atm_index + 2]);
		std::fstream commandFile;
		commandFile.open(commandFilePath, std::fstream::in);
		commandFiles.push_back(std::move(commandFile));
	}
	
	// events sceduler
	auto sched_start = std::chrono::system_clock::now();
	uint bankTimer = 0;
	uint snapshotTimer = 0;
	//uint atmToWake = 0;

	// main thread scheduler. will wake up ATM every 100ms, Bank every 3s and snapshot every 0.5s


	while(commandFiles.size() > 0) {
		auto sched_time = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = sched_time - sched_start;
		//std::cout << "current time: " << elapsed_seconds.count() << std::endl;
		// execute atm operation ever 100ms

		std::vector<pthread_t> atmThreads = std::vector<pthread_t>();

		if(elapsed_seconds.count() >= 0.1 ) {			
			
			// more commands left.. create new thread
			uint atmIndex;
			for(atmIndex = 0; atmIndex < commandFiles.size(); atmIndex++)
			{
				if(!ProcessCommand(commandFiles, atmIndex, command_sep)){
					// eof for some file.. leave atmindex the same
					commandFiles.at(atmIndex).close();
					commandFiles.erase(commandFiles.begin() + atmIndex);
					std::cout << "eof for file: " << atmIndex << std::endl;
					std::cout << "num of files: " << commandFiles.size() << std::endl;
					atmIndex--;
					continue;
					
				}
				else
				{
					executeAtmOperation
					(
						account_vec,
						command_sep,
						atmThreads,
						logger,
						atmIndex,
						globalLock
					);
				}
				
			}
			for(atmIndex = 0; atmIndex < atmThreads.size(); atmIndex++)
			{
				pthread_join(atmThreads[atmIndex], NULL);
			}
			sched_start = std::chrono::system_clock::now();
			bankTimer++;
			snapshotTimer++;
		}
		// if 30 cycles of atm operation passed, do bank operation
		if(bankTimer >= 30) {
			bankTimer = 0;
			BankOpArgs *bankArgs = (BankOpArgs *)malloc(sizeof(BankOpArgs));

			allocated_mem.push_back((void *)bankArgs);

			bankArgs->accounts_vec = &account_vec;
			bankArgs->bankAccount = &bankBalance;
			bankArgs->logObj = logger;

			pthread_t bank_op_thread;
			threadsLst.push_back(bank_op_thread);
			pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &BankOperations, (void *)bankArgs);
			pthread_join(threadsLst.at(threadsLst.size() - 1), NULL);
		}

		if(snapshotTimer >= 5)
		{
			snapshotTimer = 0;
			snapshotArgs *args = (snapshotArgs *)malloc(sizeof(snapshotArgs));
			args->accounts_vec = &account_vec;
			args->bankBalance = &bankBalance;
			pthread_t thread;
			pthread_create(&thread, NULL, &printAccountSnapshot, (void *)args);
			threadsLst.push_back(thread);
			pthread_join(threadsLst.at(threadsLst.size() - 1), NULL);
		}

		// check if some memory need to be freed
		/*
		uint thread_index;
		for (thread_index = 0; thread_index < allocated_mem.size(); thread_index++) 
		{
			int ret;
			if(thread_index >= threadsLst.size()) { break; }  
			// send empty signal the check if thread still live
			if( (ret = pthread_kill(threadsLst.at(thread_index), 0)) != 0)
			{
				// thread died, free allocated mem
				free(allocated_mem.at(thread_index));
				// remove ptr from arguments list
				allocated_mem.erase(allocated_mem.begin() + thread_index);
			}
		}
		*/
		
	}

	delete logger;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "\n\nTotal running time:\n\n " << elapsed_seconds.count() << std::endl;

	return 0;

}	

bool ProcessCommand(std::vector<std::fstream>& commandFiles, uint currentAtmMachine,
std::vector<std::string>& command_sep) 
{
	std::string command;
	std::getline(commandFiles.at(currentAtmMachine), command);
	
	if(command.empty()) {

		return false;
	}
	
	HandleInputLine(command , command_sep);
	//increment file index
	return true;
}


void HandleInputLine(std::string& inputLine, std::vector<std::string>& command_sep)
{
	const char 	*delimiters = " \t\n";
	char 		*Command = (char*)inputLine.c_str();
	const char  *cmd = strtok(Command, delimiters);
	char 		*account = strtok(NULL, delimiters);
	char 		*arg;
	// flush the vector
	command_sep.clear();
	command_sep.push_back(std::string(cmd));
	command_sep.push_back(std::string(account));
	
	do
	{
		arg = std::strtok(NULL, delimiters);
		if(arg != '\0')
		{
			command_sep.push_back(std::string(arg));
		}
	}while(arg != '\0');

}


void executeAtmOperation(std::vector<account>& account_vec,
						std::vector<std::string>& command_sep,
						std::vector<pthread_t>& active_threads,
						Logger *logger,
						int currentAtmMachine,
						RWLock *masterLock) 
{

	pthread_t current_thread;
	threadsLst.push_back(current_thread);
	
	// arguments that will be sent to each thread.
	// TODO: free the args after thread dies.
	threadArgs *args = (threadArgs *)malloc(sizeof(threadArgs));
	allocated_mem.push_back((void *)args);
	
	args->logObj = logger;
	args->atmID = currentAtmMachine + 1;
	args->myAccounts = &account_vec;
	args->globalLock = masterLock;
	args->ID = atoi(command_sep[1].c_str());
	args->password = atoi(command_sep[2].c_str());
	
	if (command_sep[0] == std::string("O")) {
		// create new account
		account *target_account = (account *)malloc(sizeof(account));
		args->amount = atoi(command_sep[3].c_str());
		target_account->accountLock = new RWLock();
		args->targetAccount = target_account;
		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::openAccout, (void *)args);		
	}
	else if (command_sep[0] == std::string("D")) {
		args->amount = atoi(command_sep[3].c_str());
		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::Deposit, (void *)args);		
	}
	else if (command_sep[0] == std::string("W")) {
		args->amount = atoi(command_sep[3].c_str());
		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::Withdrew, (void *)args);
	}
	else if (command_sep[0] == std::string("B")) {
		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::Balance, (void *)args);
	}
	else if (command_sep[0] == std::string("Q")) {

		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::closeAccount, (void *)args);
	}
	else if (command_sep[0] == std::string("T")) {
		args->targetID = atoi(command_sep[3].c_str());
		args->amount = atoi(command_sep[4].c_str());
		
		pthread_create(&threadsLst.at(threadsLst.size() - 1), NULL, &ATM::Transfer, (void *)args);
	}
	//pthread_join(threadsLst.at(threadsLst.size() - 1), NULL);
	active_threads.push_back(current_thread);
}

void *BankOperations(void *args) 
{
	BankOpArgs *bankArgs = (BankOpArgs *)args;

	int rand_tax = rand() % 4 + 2;

	uint accIndex;
	
	for (accIndex = 0; accIndex < bankArgs->accounts_vec->size(); accIndex++)
	{	
		bankArgs->accounts_vec->at(accIndex).accountLock->enterRead();
		int amount = (int)(bankArgs->accounts_vec->at(accIndex).remainer * 0.01 * rand_tax);
		int ID = bankArgs->accounts_vec->at(accIndex).ID;
		bankArgs->accounts_vec->at(accIndex).accountLock->leaveRead();

		bankArgs->accounts_vec->at(accIndex).accountLock->enterWrite();
		
		bankArgs->accounts_vec->at(accIndex).remainer -= amount;
		bankArgs->accounts_vec->at(accIndex).accountLock->leaveWrite();

		*(bankArgs->bankAccount) = *(bankArgs->bankAccount) + amount;
		bankArgs->logObj->logBankOperation(rand_tax, amount, ID);
	}
	
	return (void *)RET_SUCESS;
}

void *printAccountSnapshot(void *args)
{

	snapshotArgs *argsPtr = (snapshotArgs *)args;
	// clear the output stream
	printf("\033[2J");
	// move cursor to top left corner
	printf("\033[1;1H");
	printf("Current Bank Status\n");
	uint accIndex;
	uint size = argsPtr->accounts_vec->size();
	for (accIndex = 0; accIndex < size; accIndex++)
	{
		char padded_pass[4];
		
		Logger::padPassword(argsPtr->accounts_vec->at(accIndex).password, padded_pass);
		argsPtr->accounts_vec->at(accIndex).accountLock->enterRead();
		int remainer = argsPtr->accounts_vec->at(accIndex).remainer;
		argsPtr->accounts_vec->at(accIndex).accountLock->leaveRead();
		printf("Account %d: Balance - %d $ , Account Password - %s\n", 
			argsPtr->accounts_vec->at(accIndex).ID, remainer, padded_pass);
	}
	printf("The Bank has %d $\n", *argsPtr->bankBalance);
	return (void *)RET_SUCESS;
}


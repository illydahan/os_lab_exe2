//		skeleton .cpp
//********************************************

#include <string.h>
#include <cstring>
#include <iostream>
#include "ATM.h"


#define LOG_FILE ".log.txt"

//declaring functions in the main. may be moved.
void HandleInputLine(std::string inputLine, std::vector<std::string>& command_sep);


int main(int argc, char *argv[]) {

	if(argc < 3) {
		std::cout << "usage: ./Bank N file1 file2.." << std::endl;
		return -1;
	}
	// Parsed input command
	std::vector<std::string> command_sep = std::vector<std::string>();
	// Here we'll store all the accounts
	std::vector<account> account_vec = std::vector<account>();
	// input line (temp ?)
	std::string input;

	// ATM's list. here we'll store all the atm's.
	// each atm instance is able to perform varius atm operations.
	std::vector<ATM> atmVector = std::vector<ATM>();

	// A vector that stores all the command files
	std::vector<std::fstream> commandFiles = std::vector<std::fstream>();

	// Readers Writers lcoking mechanisem object
	RWLock *dataLock = new RWLock();

	// Logging file lock.
	pthread_mutex_t logLock;
	pthread_mutex_init(&logLock, NULL);

	// An object that will perform logging operations
	Logger *logger = new Logger(LOG_FILE);

	// here we will store all the 
	std::vector<pthread_t> threadsLst = std::vector<pthread_t>();


	uint atm_number = atoi(argv[1]);
	if(atm_number < 1) {
		std::cout << "at least one atm is required" << std::endl;
		return -1;
		// TODO: maybe not? 
	}

	// initialize all the atm objects and add them to the vector.
	for(uint atm_index = 0; atm_index < atm_number; atm_index++)
	{	
		// todo validate file (really neccessry ?).
		// first argument is the prorgam name , seconed is the number of the atm's so we'll skip those.
		std::string commandFilePath(argv[atm_index + 2]);
		std::fstream commandFile;
		commandFile.open(commandFilePath, std::fstream::in);
		commandFiles.push_back(std::move(commandFile));
	}

	// main program loop. 
	// here we'll iterate over the atm objects and execute each atm command

	for(uint index = 0; index < atm_number; index++)
	{
		std::string command;
		std::getline(commandFiles.at(index), command);

		
		if(command.empty()) {
			commandFiles.at(index).close();
			commandFiles.erase(commandFiles.begin() + index);
			continue;
		}

		HandleInputLine(command , command_sep);

		pthread_t current_thread;

		threadsLst.push_back(current_thread);

		// arguments that will be sent to each thread.
		threadArgs args = {};
		args.logObj = logger;
		args.dataLock = dataLock;
		args.logLock = &logLock;
		args.atmID = index + 1;
		args.myAccounts = account_vec;

		if (!strcmp(command_sep[0].c_str(), (const char*)'O')) {
			// create new account
			account target_account;
			target_account.ID = atoi(command_sep[1].c_str());
			target_account.password = atoi(command_sep[2].c_str());
			target_account.remainer = atoi(command_sep[3].c_str());
			args.targetAccount = target_account;
			pthread_create(&current_thread, NULL, &ATM::openAccout, (void *)&args);
			
		}
		else if (!strcmp(command_sep[0].c_str(), (const char*)'D')) {
			
			args.ID = atoi(command_sep[1].c_str());
			args.password = atoi(command_sep[2].c_str());
			args.amount = atoi(command_sep[3].c_str());

			pthread_create(&current_thread, NULL, &ATM::Deposit, (void *)&args);
			
		}
		else if (!strcmp(command_sep[0].c_str(), (const char*)'W')) {
			args.ID = atoi(command_sep[1].c_str());
			args.password = atoi(command_sep[2].c_str());
			args.amount = atoi(command_sep[3].c_str());

			pthread_create(&current_thread, NULL, &ATM::Withdrew, (void *)&args);
		}
		else if (!strcmp(command_sep[0].c_str(), (const char*)'B')) {
			args.ID = atoi(command_sep[1].c_str());
			args.password = atoi(command_sep[2].c_str());
			
			pthread_create(&current_thread, NULL, &ATM::Balance, (void *)&args);
		}
		else if (!strcmp(command_sep[0].c_str(), (const char*)'Q')) {
			args.ID = atoi(command_sep[1].c_str());
			args.password = atoi(command_sep[2].c_str());
			
			pthread_create(&current_thread, NULL, &ATM::closeAccount, (void *)&args);

		}
		else if (!strcmp(command_sep[0].c_str(), (const char*)'T')) {
			
			args.ID = atoi(command_sep[1].c_str());
			args.password = atoi(command_sep[2].c_str());
			args.targetID = atoi(command_sep[3].c_str());
			args.amount = atoi(command_sep[4].c_str());
			
			pthread_create(&current_thread, NULL, &ATM::Transfer, (void *)&args);
		}

		// reset loop
		if(atm_number == atm_number - 1) {
			atm_number = 0;
		}
	}

	delete& command_sep;
	delete& account_vec;
}	


void HandleInputLine(std::string inputLine, std::vector<std::string>& command_sep)
{
	const char 	*delimiters = " \t\n";
	char 		*Command = (char*)inputLine.c_str();
	const char  *cmd = strtok(Command, delimiters);
	char 		*account = strtok(NULL, delimiters);
	char 		*arg;
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

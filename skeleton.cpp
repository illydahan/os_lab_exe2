//		skeleton .cpp
//********************************************
#include "simple.h"
#include <vector>

//declaring functions in the main. may be moved.
void HandleInputLine(std::string inputLine, std::vector<std::string>& command_sep);


int main() {
	std::vector<std::string> command_sep = std::vector<std::string>();
	std::vector<account> account_vec = std::vector<account>();
	std::string input;
	//we'll change 'cin' to other source, obviously.
	std::cin >> input;
	HandleInputLine(input, command_sep);

	int flag = 0;
	//a critical place. when a thread searches for an account in this 
	account* the_account;
	for (account index : account_vec) {
		if (index.ID = (int)command_sep[1].c_str()) {
			flag = 1;
			the_account = &index;
			break;
		}
	}

	if (flag != 1) {
		if (!strcmp(command_sep[0].c_str(), (const char*)'O')) {
			//add here the call to the function.
			//then, return SUCCESS
		}
		std::cout << "Error " << "ATM ID TO HERE PLEASE" << ": Your transaction failed – account id ";
		std::cout << command_sep[1] << "does not exist" << std::endl;
		return 1;
	}
	flag = 0;

	//here we know that the account exists, and we search for its command
	if (!strcmp(command_sep[0].c_str(), (const char*)'O')) {
		std::cout << "Error " << "ATM ID TO HERE PLEASE" << ": Your transaction failed – account with same id exists";
		std::cout << std::endl;
		return 1;
	}
	if (!strcmp(command_sep[0].c_str(), (const char*)'D')) {
		if (!Deposit(*the_account, (int)command_sep[2].c_str(), (int)command_sep[3].c_str())) {
			return 0;
		}
		return 1;
	}
	if (!strcmp(command_sep[0].c_str(), (const char*)'B')) {
		if (!Balance(*the_account, (int)command_sep[2].c_str())) {
			return 0;
		}
		return 1;
	}
	if (!strcmp(command_sep[0].c_str(), (const char*)'T')) {
		account* other_account;
		for (account index : account_vec) {
			if (index.ID = (int)command_sep[1].c_str()) {
				flag = 1;
				other_account = &index;
				break;
			}
		}
		if (flag != 1) {
			std::cout << "Error " << "ATM ID TO HERE PLEASE" << ": Your transaction failed – account id ";
			std::cout << command_sep[1] << "does not exist" << std::endl;
			return 1;
		}
		if (!Transfer(*the_account, (int)command_sep[2].c_str(), *other_account, (int)command_sep[4].c_str())) {
			return 0;
		}
		return 1;
	}
	delete &command_sep;
	delete& account_vec;
}


void HandleInputLine(std::string inputLine, std::vector<std::string>& command_sep)
{
	const char* delimiters = " \t\n";
	char* Command = (char*)inputLine.c_str();
	const char* cmd = std::strtok(Command, delimiters);
	char* account = std::strtok(NULL, delimiters);
	char* arg;
	command_sep.push_back(cmd);
	command_sep.push_back(account);
	do
	{
		arg = std::strtok(NULL, delimiters);
		if (arg != '\0')
		{
			command_sep.push_back(arg);
		}
	} while (arg != '\0');

}
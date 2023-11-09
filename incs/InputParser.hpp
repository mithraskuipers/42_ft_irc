#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

class InputParser;

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "./../incs/Command.hpp"

class Command;

class InputParser
{
	private:
		Server *server;
		std::map<std::string, Command *> commandPortal;
		// Function to process a command for a client
		void processCurrentLine(Client *client, const std::string &line);

		// Function to trim trailing carriage return from input
		std::string trimTrailingCarriageReturn(const std::string &input);

		// Function to extract command name from line
		std::string extractCommandName(const std::string &line);

		// Function to extract command arguments from line
		std::vector<std::string> extractCommandArguments(const std::string &line, const std::string &name);

		// Function to get the command instance based on command name
		Command *getCommandInstance(const std::string &name);

	public:
		InputParser(Server *server);

		~InputParser();

		// Function to invoke command processing for a client and message
		void invoke(Client *client, const std::string &message);
};

#endif // COMMANDHANDLER_HPP

/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CommandProcessor.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:27 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:29 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/CommandProcessor.hpp"
#include "./../incs/cmdPrivmsg.hpp"
#include "./../incs/cmdPart.hpp"
#include "./../incs/cmdPass.hpp"
#include "./../incs/cmdNick.hpp"
#include "./../incs/cmdJoin.hpp"
#include "./../incs/cmdPing.hpp"
#include "./../incs/cmdKick.hpp"
#include "./../incs/cmdPong.hpp"
#include "./../incs/cmdUser.hpp"
#include "./../incs/cmdTopic.hpp"
#include "./../incs/Replies.hpp"
#include "./../incs/cmdInvite.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

// Volledig gescheiden van elkaar dus je kunt zonder teveel issues een nieuwe command toevoegen.

CommandProcessor::CommandProcessor(Server *server) //: server(server)
{
	commandPortal["PASS"] = new cmdPass(server, false);
	commandPortal["NICK"] = new cmdNick(server, false);
	commandPortal["USER"] = new cmdUser(server, false);
	commandPortal["PING"] = new cmdPing(server, true); // Crucial for maintaining connection
	commandPortal["PONG"] = new cmdPong(server, true); // Crucial for maintaining connection
	commandPortal["JOIN"] = new cmdJoin(server, true);
	commandPortal["TOPIC"] = new cmdTopic(server, true);
	commandPortal["PART"] = new cmdPart(server, true);
	commandPortal["KICK"] = new cmdKick(server, true);
	commandPortal["INVITE"] = new cmdInvite(server, true);
	commandPortal["PRIVMSG"] = new cmdPrivmsg(server, true);
}

void CommandProcessor::process(Client *client, const std::string &firstMessageCombined)
{
	std::stringstream tokenizedMessage(firstMessageCombined); // "Tokenizes" original string in separate strings, using \n as delimiter.
	std::string currentMessageLine;							  // Temp var for working with each token

	while (std::getline(tokenizedMessage, currentMessageLine)) // Obtain token line from tokenizdMessage
	{
		processCurrentLine(client, currentMessageLine); // Process current token line
	}
}

void CommandProcessor::processCurrentLine(Client *client, const std::string &line)
{
	std::string modifiedLine;
	std::string commandName;
	std::vector<std::string> commandArguments;

	modifiedLine = trimTrailingCarriageReturn(line);
	commandName = extractCommandName(modifiedLine);
	try
	{
		Command *command = getCommandInstance(commandName);						// This is important. This retrieves the actual command (instance) of interest using only the command name.
		commandArguments = extractCommandArguments(modifiedLine, commandName);	// Extract the command arguments


		if (!client->isRegistered() && command->registrationRequired())			// Check if the command can be execute without being fully registrered. Might need work.
		{
			client->sendMessageToClientItself(ERR_NOTREGISTERED(client->getNickname()));
			return;
		}

		command->execute(client, commandArguments);								// Run the command instance using execute() member function and arguments.
	}
	catch (const std::out_of_range &e)
	{
		client->sendMessageToClientItself(ERR_UNKNOWNCOMMAND(client->getNickname(), commandName));
	}
}

std::string CommandProcessor::trimTrailingCarriageReturn(const std::string &input)
{
	std::string trimmedString = input;
	return trimmedString.substr(0, trimmedString.back() == '\r' ? trimmedString.length() - 1 : trimmedString.length());
}

std::string CommandProcessor::extractCommandName(const std::string &line)
{
	return (line.substr(0, line.find(' ')));
}

std::vector<std::string> CommandProcessor::extractCommandArguments(const std::string &line, const std::string &commandName)
{
	std::vector<std::string>	commandArguments;
	std::string					buffer;
	std::stringstream ss(line.substr(commandName.length(), line.length()));

	while (ss >> buffer)
	{
		commandArguments.push_back(buffer);
	}

	return (commandArguments);
}

Command *CommandProcessor::getCommandInstance(const std::string &commandName)
{
	return (commandPortal.at(commandName));
}

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

CommandProcessor::~CommandProcessor()
{
	for (auto &commandPair : commandPortal)
	{
		delete (commandPair.second);											// Because its a map..
	}
}

/*
********************************************************************************
GETTERS / SETTERS
********************************************************************************
*/

// Not Applicable 8-)
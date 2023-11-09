/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   InputParser.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 12:30:40 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/09 22:17:17 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/InputParser.hpp"
#include "./../incs/cmdPrivmsg.hpp"
#include "./../incs/cmdPart.hpp"
#include "./../incs/cmdPass.hpp"
#include "./../incs/cmdNick.hpp"
#include "./../incs/cmdJoin.hpp"
#include "./../incs/cmdPing.hpp"
#include "./../incs/cmdPong.hpp"
#include "./../incs/cmdUser.hpp"
#include "./../incs/Replies.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

// Volledig gescheiden van elkaar dus je kunt zonder issues een nieuwe command toevoegen.


InputParser::InputParser(Server *server) : server(server)
{
	commandPortal["PASS"] = new cmdPass(server, false);
	commandPortal["NICK"] = new cmdNick(server, false);
	commandPortal["USER"] = new cmdUser(server, false);
	commandPortal["PING"] = new cmdPing(server, true); // Crucial for maintaining connection
	commandPortal["PONG"] = new cmdPong(server, true); // Crucial for maintaining connection
	commandPortal["JOIN"] = new cmdJoin(server, true);
	commandPortal["PART"] = new cmdPart(server, true);
	commandPortal["PRIVMSG"] = new cmdPrivmsg(server, true);
}

void InputParser::invoke(Client *client, const std::string &firstMessageCombined)
{
	std::stringstream	tokenizedMessage(firstMessageCombined);								// "Tokenizes" original string in separate strings, using \n as delimiter.
	std::string			currentMessageLine;										// Temp var for working with each token

	while (std::getline(tokenizedMessage, currentMessageLine))					// Obtain token line from tokenizdMessage
	{
		processCurrentLine(client, currentMessageLine);							// Process current token line
	}
}

void InputParser::processCurrentLine(Client *client, const std::string &line)
{
	std::string modifiedLine;
	std::string name;

	modifiedLine = trimTrailingCarriageReturn(line);
	name = extractCommandName(modifiedLine);
	try
	{
		Command *command = getCommandInstance(name);

		if (!client->isRegistered() && command->registrationRequired())
		{
			client->sendMessageToClientItself(ERR_NOTREGISTERED(client->getNickname()));
			return ;
		}

		std::vector<std::string> arguments = extractCommandArguments(modifiedLine, name);
		command->run(client, arguments);
	}
	catch (const std::out_of_range &e)
	{
		client->sendMessageToClientItself(ERR_UNKNOWNCOMMAND(client->getNickname(), name));
	}
}

std::string InputParser::trimTrailingCarriageReturn(const std::string &input)
{
    std::string trimmedString = input;
    return trimmedString.substr(0, trimmedString.back() == '\r' ? trimmedString.length() - 1 : trimmedString.length());
}

std::string InputParser::extractCommandName(const std::string &line)
{
	return line.substr(0, line.find(' '));
}

std::vector<std::string> InputParser::extractCommandArguments(const std::string &line, const std::string &name)
{
	std::vector<std::string> arguments;
	std::string buf;
	std::stringstream ss(line.substr(name.length(), line.length()));

	while (ss >> buf)
	{
		arguments.push_back(buf);
	}
	
	return (arguments);
}

Command* InputParser::getCommandInstance(const std::string &name)
{
	return commandPortal.at(name);
}

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

InputParser::~InputParser()
{
	for (auto &commandPair : commandPortal)
	{
		delete commandPair.second;
	}
}

/*
********************************************************************************
GETTERS / SETTERS
********************************************************************************
*/

// Not Applicable 8-)
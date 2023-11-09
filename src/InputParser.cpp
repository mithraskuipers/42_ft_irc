/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   InputParser.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 12:30:40 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/08 19:40:00 by mikuiper      ########   odam.nl         */
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
	_commands["PASS"] = new cmdPass(server, false);
	_commands["NICK"] = new cmdNick(server, false);
	_commands["USER"] = new cmdUser(server, false);
	_commands["PING"] = new cmdPing(server, true); // Crucial for maintaining connection
	_commands["PONG"] = new cmdPong(server, true); // Crucial for maintaining connection
	_commands["JOIN"] = new cmdJoin(server, true);
	_commands["PART"] = new cmdPart(server, true);
	_commands["PRIVMSG"] = new cmdPrivmsg(server, true);
}

void InputParser::invoke(Client *client, const std::string &message)
{
	std::stringstream	tokenizedMessage(message);								// "Tokenizes" original string in separate strings, using \n as delimiter.
	std::string			currentMessageLine;										// Temp var for working with each token

	while (std::getline(tokenizedMessage, currentMessageLine))					// Obtain token line from tokenizdMessage
	{
		processCurrentLine(client, currentMessageLine);							// Process current token line
	}
}

// TODO

// TODO SYNTAX?

void InputParser::processCurrentLine(Client *client, const std::string &syntax)
{
	std::string modifiedSyntax;
	std::string name;

	modifiedSyntax = trimTrailingCarriageReturn(syntax);
	name = extractCommandName(modifiedSyntax);
	try
	{
		Command *command = getCommandInstance(name);

		if (!client->isRegistered() && command->registrationRequired())
		{
			client->sendMessageToClientItself(ERR_NOTREGISTERED(client->getNickname()));
			return ;
		}

		std::vector<std::string> arguments = extractCommandArguments(modifiedSyntax, name);
		command->run(client, arguments);
	}
	catch (const std::out_of_range &e)
	{
		client->sendMessageToClientItself(ERR_UNKNOWNCOMMAND(client->getNickname(), name));
	}
}

// TODO

std::string InputParser::trimTrailingCarriageReturn(const std::string &input)
{
    std::string trimmedString = input;
    return trimmedString.substr(0, trimmedString.back() == '\r' ? trimmedString.length() - 1 : trimmedString.length());
}

// TODO

std::string InputParser::extractCommandName(const std::string &syntax)
{
	return syntax.substr(0, syntax.find(' '));
}

// TODO

std::vector<std::string> InputParser::extractCommandArguments(const std::string &syntax, const std::string &name)
{
	std::vector<std::string> arguments;
	std::string buf;
	std::stringstream ss(syntax.substr(name.length(), syntax.length()));

	while (ss >> buf)
	{
		arguments.push_back(buf);
	}
	
	return (arguments);
}

// TODO

Command* InputParser::getCommandInstance(const std::string &name)
{
	return _commands.at(name);
}

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

// TODO

InputParser::~InputParser()
{
	for (auto &commandPair : _commands)
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
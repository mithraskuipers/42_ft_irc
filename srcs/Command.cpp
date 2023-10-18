/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:54:57 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/18 23:54:00 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "includes.hpp"
#include <string>
#include <vector>
#include <sstream>

Command::Command()
{
	// POPULATE DE COMMANDDICTIONARY MET FUNCTIES ZOALS /NICK EN /JOIN
	// @ RICK MISSCHIEN KUN JE JE CODE HIER NAAR OVERZETTEN?
// Setter function to set the client's IP address
}

Command::~Command()
{
}

std::vector<std::string> Command::splitCmd(const std::string& input, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t end = input.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
		end = input.find(delimiter, start);
	}

	tokens.push_back(input.substr(start));
	return tokens;
}

// Maakt string in format dat wordt verwacht door IRC servers en clients.
void Command::sendMessage(User& client, const std::string& msgCode, const std::string& target, const std::string& message)
{
	std::string response = ":" + std::string(SERVER_NAME) + " " + msgCode + " " + client.getNick() + " " + target + " :" + message + "\r\n";
	std::cout << "CREATED RESPONSE FOR SERVER: " << response << std::endl;
	send(client.getSocketDescriptor(), response.c_str(), response.size(), 0);
}

void Command::processCommand(User& client)
{
	std::string buffer = client.getBuff();

	// Check if the buffer contains '\r\n' or '\n' as the command delimiter
	std::string delimiter = (buffer.find("\r\n") != std::string::npos) ? "\r\n" : "\n";
	std::vector<std::string> cmdLines = splitCmd(buffer, delimiter);

	for (const std::string& cmdLine : cmdLines)
	{
		std::vector<std::string> cmds = splitCmd(cmdLine, " ");

		// Handle messages starting with ':'
		if (cmds[0][0] == ':')
		{
			cmds[0].erase(0, 1);
			if (cmds[0] != client.getNick())
			{
				// Ignore messages not addressed to the client
				continue;
			}
			cmds.erase(cmds.begin());
		}

		std::string command = cmds[0];
		std::transform(command.begin(), command.end(), command.begin(), toupper);

		// List of commands. Iterate throughlist and find match.
		auto iter = commandDictionary.find(command);							// Find key matching with command

		if (iter != commandDictionary.end())
		{
			(this->*(iter->second))(cmds, client);								// Remember "second" takes the value from key-value pairs. Command name: Command function.
		}
		else if (client.getRegisteredStatus())									// Simpele error management.
		{
			sendMessage(client, "421", command, "nu mag ik je een wanta geven 🤣🤣🤣"); // Error code 421 staat schijnbaar voor unknown command.
		}
	}
}

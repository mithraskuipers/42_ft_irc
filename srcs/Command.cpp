/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:54:57 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 19:55:45 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "includes.hpp"
#include <string>
#include <vector>
#include <sstream>

/*
Orthodox Canonical Form
*/

Command::Command()
{
	commandDictionary["JOIN"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->joinCommand(cmds, Client);
	};
	commandDictionary["CREATE"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->createCommand(cmds, Client);
	};
	commandDictionary["LIST"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->listCommand(cmds, Client);
	};
	commandDictionary["CREATE"] = [this](const std::vector<std::string>& cmds, User& Client) {
    this->createCommand(cmds, Client);
};

}

Command::~Command()
{
}

/*
Member functions
*/


void trim(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}


std::vector<std::string> Command::splitCmd(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    // Find the first occurrence of the delimiter
    end = input.find(delimiter, start);

    // Iterate until no more delimiters are found
    while (end != std::string::npos) {
        // Extract the token between start and end positions
        std::string token = input.substr(start, end - start);

        // Add the token to the vector
        tokens.push_back(token);

        // Move the start position to the character after the last delimiter
        start = end + delimiter.length();

        // Find the next occurrence of the delimiter
        end = input.find(delimiter, start);
    }

    // Add the last token after the last delimiter (or the whole input if no more delimiters found)
    std::string lastToken = input.substr(start);
    tokens.push_back(lastToken);

    return tokens;
}


void Command::sendMessage(User& Client, const std::string& msgCode, const std::string& target, const std::string& message)
{
    std::string response = ":" + std::string(SERVER_NAME) + " " + msgCode + " " + Client.getNick() + " " + target + " :" + message + "\r\n";
    std::cout << "CREATED RESPONSE FOR SERVER: " << response << std::endl;
    send(Client.getSocketDescriptor(), response.c_str(), response.size(), 0);
}


void Command::commandHandler(User& Client)
{
    std::string buffer = Client.getBuff();
	// std::cout << buffer << std::endl;

    // Split commands based on newline delimiter
	std::vector<std::string> cmdLines = splitCmd(buffer, "\n");
	
	// Check if cmdLines vector is not empty before accessing the first element
	// if (!cmdLines.empty())
	// {
	//     std::cout << "First element of cmdLines: " << cmdLines[0] << std::endl;
	// }
	// else
	// {
	//     std::cout << "cmdLines vector is empty." << std::endl;
	// }
	
    for (const std::string& cmdLine : cmdLines)
    {
        std::vector<std::string> cmds = splitCmd(cmdLine, " ");

        if (cmds.empty())
        {
            // Empty command, skip
			std::cout << "Commando was leeg.." << std::endl;
            continue;
        }
		std::cout << cmds[0] << std::endl;										// Kijk naar het eerste stuk
	

        // std::string command = cmds[1]; // The command is now at index 1 in cmds after removing the prefix
        // std::transform(command.begin(), command.end(), command.begin(), toupper);
        // auto iter = commandDictionary.find(command);
		/*
        if (iter != commandDictionary.end()) {
            iter->second(cmds, Client); // Call the lambda function directly
        } else {
            sendMessage(Client, "421", Client.getNick(), "Unknown command");
        }
	*/
    }

}




void Command::createCommand(const std::vector<std::string>& cmds, User& Client) {
    std::string channelName = cmds[2]; // Extract channel name from the command

    // Check if the channel already exists
    auto it = channels.find(channelName);
    if (it != channels.end()) {
        sendMessage(Client, "CREATE", channelName, "Channel " + channelName + " already exists");
    }
	else
	{
        // Create a new channel and add the user
        std::vector<User> newChannel;
        newChannel.push_back(Client);
        channels[channelName] = newChannel;
        sendMessage(Client, "CREATE", channelName, "Channel " + channelName + " created successfully");
    }
}






void Command::joinCommand(const std::vector<std::string>& cmds, User& Client) {
    std::string channelName = cmds[2]; // Extract channel name from the command

    // Check if the channel already exists
    auto it = channels.find(channelName);
    if (it != channels.end()) {
        // Add the user to an existing channel
        it->second.push_back(Client);
    } else {
        // Create a new channel and add the user
        std::vector<User> newChannel;
        newChannel.push_back(Client);
        channels[channelName] = newChannel;
    }

    // Example response to send back to the User
    sendMessage(Client, "JOIN", channelName, "Welcome to Channel " + channelName);
}



void Command::listCommand(const std::vector<std::string>& cmds, User& Client) {
    (void)cmds;

    std::string channelList = "Channels: ";
    for (const auto& pair : channels) {
        // pair.first is the Channel name, pair.second.size() is the number of users in the Channel
        channelList += pair.first + "(" + std::to_string(pair.second.size()) + ") ";
    }

    // Send the Channel list back to the User
    sendMessage(Client, "LIST", Client.getNick(), channelList);
}

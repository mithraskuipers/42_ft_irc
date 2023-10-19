/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:54:57 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 12:45:23 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "includes.hpp"
#include <string>
#include <vector>
#include <sstream>

Command::Command()
{
    commandDictionary["JOIN"] = [this](const std::vector<std::string>& cmds, User& client) {
        this->joinCommand(cmds, client);
    };
    commandDictionary["CREATE"] = [this](const std::vector<std::string>& cmds, User& client) {
        this->createCommand(cmds, client);
    };
    commandDictionary["LIST"] = [this](const std::vector<std::string>& cmds, User& client) {
        this->listCommand(cmds, client);
    };
}

void Command::commandHandler(User& client) {
    std::string buffer = client.getBuff();

    // Check if the buffer contains '\r\n' or '\n' as the command delimiter
    std::string delimiter = (buffer.find("\r\n") != std::string::npos) ? "\r\n" : "\n";
    std::vector<std::string> cmdLines = splitCmd(buffer, delimiter);

    for (const std::string& cmdLine : cmdLines) {
        std::vector<std::string> cmds = splitCmd(cmdLine, " ");

        // Handle messages starting with ':'
        if (cmds[0][0] == ':') {
            cmds[0].erase(0, 1);
            if (cmds[0] != client.getNick()) {
                // Ignore messages not addressed to the client
                continue;
            }
            cmds.erase(cmds.begin());
        }

        std::string command = cmds[0];
        std::transform(command.begin(), command.end(), command.begin(), toupper);

        auto iter = commandDictionary.find(command);
        if (iter != commandDictionary.end()) {
            iter->second(cmds, client); // Call the lambda function directly
        } else if (client.getRegisteredStatus()) {
            sendMessage(client, "421", command, "nu mag ik je een wanta geven 🤣🤣🤣");
        }
    }
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

void Command::joinCommand(const std::vector<std::string>& cmds, User& client) {
    // Assuming cmds[1] contains the Channel name to join
    std::string channelName = cmds[1];
    // Implement logic to join the Channel with the given name
    // For simplicity, let's assume a map where keys are Channel names and values are vectors of User objects.
    std::map<std::string, std::vector<User>> channels;

    // Check if the Channel exists, if not, create it
    if (channels.find(channelName) == channels.end())
	{
        channels[channelName] = std::vector<User>();
    }

    // Add the User to the Channel
    channels[channelName].push_back(client);

    // Example response to send back to the client
    sendMessage(client, "JOIN", channelName, "Welcome to Channel " + channelName);
}

void Command::createCommand(const std::vector<std::string>& cmds, User& client) {
    // Assuming cmds[1] contains the Channel name to create
    std::string channelName = cmds[1];
    // Implement logic to create a new Channel with the given name
    // For simplicity, let's assume a map where keys are Channel names and values are vectors of User objects.
    std::map<std::string, std::vector<User>> channels;

    // Check if the Channel already exists
    if (channels.find(channelName) != channels.end()) {
        sendMessage(client, "CREATE", channelName, "Channel " + channelName + " already exists");
    } else {
        // Create the new Channel
        channels[channelName] = std::vector<User>();
        // Add the User to the Channel
        channels[channelName].push_back(client);
        sendMessage(client, "CREATE", channelName, "Channel " + channelName + " created successfully");
    }
}

void Command::listCommand(const std::vector<std::string>& cmds, User& client)
{
	(void)cmds;

    // Implement logic to retrieve the list of available channels
    // For simplicity, let's assume a map where keys are Channel names and values are vectors of User objects.
    std::map<std::string, std::vector<User>> channels;

    std::string channelList = "Channels: ";
    for (const auto& pair : channels) {
        // pair.first is the Channel name, pair.second.size() is the number of users in the Channel
        channelList += pair.first + "(" + std::to_string(pair.second.size()) + ") ";
    }

    // Send the Channel list back to the client
    sendMessage(client, "LIST", client.getNick(), channelList);
}


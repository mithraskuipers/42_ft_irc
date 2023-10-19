/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:54:57 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 21:00:30 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "includes.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <functional>


/*
Orthodox Canonical Form
*/


#include "Command.hpp"
#include "includes.hpp"
#include <iostream>
#include <sstream>

Command::Command() {
	commandDictionary["JOIN"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->joinCommand(cmds, Client);
	};
	commandDictionary["CREATE"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->createCommand(cmds, Client);
	};
	commandDictionary["LIST"] = [this](const std::vector<std::string>& cmds, User& Client) {
		this->listCommand(cmds, Client);
	};
}

Command::~Command() {}

void Command::trim(std::string& str) {
	// Trim leading and trailing spaces from the string
	str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
	str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}


std::vector<std::string> Command::splitCmd(const std::string& input) {
	std::vector<std::string> tokens;
	std::istringstream iss(input);
	std::string token;

	while (std::getline(iss, token, ' ')) {
		this->trim(token); // Corrected here
		if (!token.empty()) {
			tokens.push_back(token);
		}
	}

	return tokens;
}

void Command::sendMessage(User& Client, const std::string& msgCode, const std::string& target, const std::string& message) {
	std::string response = ":" + std::string(SERVER_NAME) + " " + msgCode + " " + Client.getNick() + " " + target + " :" + message + "\r\n";
	std::cout << "CREATED RESPONSE FOR SERVER: " << response << std::endl;
	send(Client.getSocketDescriptor(), response.c_str(), response.size(), 0);
}

void Command::commandHandler(User& Client) {
	std::string buffer = Client.getBuff();
	std::vector<std::string> cmdLines = splitCmd(buffer);

	for (const std::string& cmdLine : cmdLines) {
		std::vector<std::string> tokens = splitCmd(cmdLine);
		if (tokens.empty()) {
			std::cout << "Command was empty.." << std::endl;
			continue;
		}

		auto iter = commandDictionary.find(tokens[0]);
		if (iter != commandDictionary.end()) {
			std::cout << "Recognized command: " << tokens[0] << std::endl;
			iter->second(tokens, Client);
		} else {
			sendMessage(Client, "421", Client.getNick(), "Unknown command");
		}
	}
}

void Command::createCommand(const std::vector<std::string>& cmds, User& Client) {
	if (cmds.size() >= 2) {
		std::string command = cmds[0];
		std::string channelName = cmds[1];

		if (command == "CREATE" && !channelName.empty()) {
			auto it = channels.find(channelName);
			if (it != channels.end()) {
				sendMessage(Client, "CREATE", channelName, "Channel " + channelName + " already exists");
			} else {
				std::vector<User> newChannel;
				newChannel.push_back(Client);
				channels[channelName] = newChannel;
				sendMessage(Client, "CREATE", channelName, "Channel " + channelName + " created successfully");
			}
		} else {
			sendMessage(Client, "ERROR", Client.getNick(), "Invalid CREATE command format");
		}
	}
}

void Command::joinCommand(const std::vector<std::string>& cmds, User& Client) {
	if (cmds.size() >= 3) {
		std::string channelName = cmds[2];
		auto it = channels.find(channelName);
		if (it != channels.end()) {
			it->second.push_back(Client);
		} else {
			std::vector<User> newChannel;
			newChannel.push_back(Client);
			channels[channelName] = newChannel;
		}
		sendMessage(Client, "JOIN", channelName, "Welcome to Channel " + channelName);
	} else {
		sendMessage(Client, "ERROR", Client.getNick(), "Invalid JOIN command format");
	}
}

void Command::listCommand(const std::vector<std::string>& cmds, User& Client) {
	(void)cmds;
	std::string channelList = "Channels: ";
	for (const auto& pair : channels) {
		channelList += pair.first + "(" + std::to_string(pair.second.size()) + ") ";
	}
	sendMessage(Client, "LIST", Client.getNick(), channelList);
}

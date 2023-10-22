// Command.hpp

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <string>
#include "User.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <vector>
#include <string>

class IRCServer; // Forward declaration of IRCServer class

class Command
{
public:
	Command(std::vector<User> &clients, IRCServer &server);
	~Command();
	void addChannel(const std::string &channelName);
	void commandHandler(const std::string &input, User &client);
	bool leaveChannel(const std::string &channelName, User &user);
	bool joinChannel(const std::string &channelName, User &user);

private:
	std::vector<User> &clients;
	std::vector<Channel> channels;
	IRCServer &ircServer;

	void handlePassCommand(const std::vector<std::string> &command, User &client);
	void handleNickCommand(const std::vector<std::string> &command, User &client);
	void handleListCommand(const std::vector<std::string> &command, User &client);

	void handleUserCommand(const std::vector<std::string> &command, User &client);
	void handleQuitCommand(const std::vector<std::string> &command, User &client);
	void handleJoinCommand(const std::vector<std::string> &command, User &client);
	void handleLeaveCommand(const std::vector<std::string> &command, User &client);
	void handleCreateCommand(const std::vector<std::string> &command, User &client);
	void sendChannelList(User &user);
};

#endif // COMMAND_HPP

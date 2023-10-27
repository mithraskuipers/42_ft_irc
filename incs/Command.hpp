#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include <unordered_set>

class Channel; // Forward declaration of Channel class
class Server;  // Forward declaration of Server class

class Command
{
public:
	Command(std::vector<Client> &clients, std::vector<Channel> &channels, Server &server);
	~Command();
	void addChannel(const std::string &channelName);
	void processRawClientData(const std::string &input, Client &client);
	bool leaveChannel(const std::string &channelName, Client &client);
	bool joinChannel(const std::string &channelName, Client &client);
	bool isNicknameInUse(const std::string &nickname);
	std::unordered_set<std::string> registeredNicknames;
	bool isNicknameValid(const std::string &nickname);

private:
	std::vector<Client> &clients;
	std::vector<Channel> &channels;
	Server &ircServer;

	void privatmsg(std::vector<std::string> cmds, Client &sender);
	void handlePartCommand(const std::vector<std::string> &command, Client &client);
	void handlePassCommand(const std::vector<std::string> &command, Client &client);
	void handleWhoisCommand(const std::vector<std::string> &command, Client &client);
	void handleNickCommand(const std::vector<std::string> &command, Client &client);
	void handleListCommand(const std::vector<std::string> &command, Client &client);
	void handleClientCommand(const std::vector<std::string> &command, Client &client);
	void handleQuitCommand(const std::vector<std::string> &command, Client &client);
	void handleJoinCommand(const std::vector<std::string> &command, Client &client);
	void handleLeaveCommand(const std::vector<std::string> &command, Client &client);
	void sendChannelList(Client &client);
};

#endif
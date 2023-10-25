#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Channel; // Forward declaration of Channel class
class Server;  // Forward declaration of Server class

class Command
{
	public:
		Command(std::vector<Client> &clients, std::vector<Channel> &channels, Server &server);
		~Command();
		void addChannel(const std::string &channelName);
		void processRawClientData(const std::string &input, Client &client);
		bool leaveChannel(const std::string &channelName, Client &user);
		bool joinChannel(const std::string &channelName, Client &user);
		bool isNicknameInUse(const std::string &nickname);


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
		void handleUserCommand(const std::vector<std::string> &command, Client &client);
		void handleQuitCommand(const std::vector<std::string> &command, Client &client);
		void handleJoinCommand(const std::vector<std::string> &command, Client &client);
		void handleLeaveCommand(const std::vector<std::string> &command, Client &client);
		void sendChannelList(Client &user);
};

#endif
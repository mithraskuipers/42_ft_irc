#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "Command.hpp"
#include "includes.hpp"
#include <netinet/in.h>
#include <list>
#include "Channel.hpp"
#include "Client.hpp"

class Command;
class Client;

class Server
{
	private:
		std::string _password;
		unsigned int _activeClients;
		int _port;
		Command _command;
		int _serverListeningSocket;
		std::string _IP;
		struct sockaddr_in _socketAddress;
		std::vector<Client> _clients;
		std::vector<Channel> _channels;
		std::string _welcomeMessage;
	
	public:
		Server(int port, const std::string &password); // Constructor takes port and password as arguments
		~Server();
		
		void setMachineIP();
		void startServer();
		void initServer();

		void sendMotdMessage(int clientSocket, const std::string &userName);
		int isNickNameInUse(const std::string &nickName) const;
		void checkWhatReceivedFromClient(int clientSocket);
		std::string clientNameFromSocket(int clientSocket);
		std::string addClientSocket(int clientSocket);
		int handleNewConnection(int clientSocket);
		std::string generateRandomCode();
		int updateMaxSocketDescriptor();
		
		Client &getClientByClientName(const std::string &clientName);
		std::string getClientIP(int clientSocket);
		int getClientPort(int clientSocket);
		std::string getIP() const;
		std::string getPass();
		int getPort() const;
};

#endif

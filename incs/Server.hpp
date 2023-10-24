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
		std::string password;
		unsigned int active_users;
		int port;
		Command command;
		int server_listening_socket;
		std::string IP;
		struct sockaddr_in socket_address;
		std::vector<Client> clients;
		std::vector<Channel> channels;
		std::string welcomeMessage;
	
	public:
		void handleNewConnection(int client_socket);
		void checkIfDataReceivedFromClient(int client_socket, std::vector<char> &buffer);
	
		std::string getPass();
		Server(int port, const std::string &password); // Constructor takes port and password as arguments
		~Server();
		void initServer();
		void startServer();
		void getHostIP();
		std::string getIP() const;
		Client &getClientByUsername(const std::string &username);
		std::string getClientIP(int clientSocket);
		int getClientPort(int clientSocket);
		std::string usernameFromSocket(int clientSocket);
		int updateMaxSocketDescriptor();
		int getPort() const;
		std::string addClientSocket(int clientSocket);
		std::string generateRandomCode();
		int isNicknameInUse(const std::string &nickname) const;
		void sendMotdMessage(int client_socket, const std::string &username);
};

#endif

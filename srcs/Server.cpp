/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/25 08:01:52 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/25 08:26:53 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>
#include "./../incs/Server.hpp"
#include "./../incs/Command.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/Channel.hpp"

class Server;

void Server::initServer()
{
	int opt = 1;
	this->server_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_listening_socket == 0)
	{
		throw(std::runtime_error("Failed to create socket"));
	}
	int setsockopt_ret = setsockopt(server_listening_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (setsockopt_ret == -1)
	{
		throw(std::runtime_error("Failed to set socket options"));
	}
	this->socket_address.sin_family = AF_INET;
	this->socket_address.sin_addr.s_addr = INADDR_ANY;
	this->socket_address.sin_port = htons(port);

	if (fcntl(server_listening_socket, F_SETFL, O_NONBLOCK) < -1)
	{
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	if (bind(server_listening_socket, (struct sockaddr *)&this->socket_address, sizeof(this->socket_address)) < 0)
	{
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(server_listening_socket, QUEUE_SIZE) == -1)
	{
		throw std::runtime_error("Failed to start listening");
	}

	std::cout << "Listening on port: " << this->port << std::endl;
	std::cout << "The password you chose to use is: " << this->password << std::endl;
}

void Server::startServer()
{
	// Infinite loop om server actief te houden
	while (true) # WIP
	{
		// Vector voor poll structs
		std::vector<pollfd> fds;

		// Voeg poll struct toe aan bovenstaande vector. Configureer deze voor de server.
		pollfd serverSocket;

		serverSocket.fd = server_listening_socket;
		serverSocket.events = POLLIN;
		fds.push_back(serverSocket);

		// Voeg meer poll structs toe aan bovenstaande vector. Configureer deze voor de clients.
		for (const auto &Client : clients)
		{
			// pollfd clientSocket;
			// clientSocket.fd = Client.getSocketDescriptor();
			// clientSocket.events = POLLIN;
			fds.push_back(Client.getPolledFD());
		}

		// Voer poll() uit op de fds in de vector en kijk of error
		if (poll(fds.data(), fds.size(), -1) == -1)
		{
			std::cerr << "Error: Failed to perform poll() on socket: " << strerror(errno) << std::endl;
			break;
		}

		// Iterate over readySockets en process events
		for (size_t i = 0; i < fds.size(); ++i)
		{
			int returned_events;
			returned_events = fds[i].revents; // Dit zijn events die werkelijk zijn gebeurd
			// POLLIN heeft plaatsgevonden en fd komt overeen met server. betekent POLLIN van nieuwe client
			if ((returned_events & POLLIN) && (fds[i].fd == server_listening_socket))
			{
				// probeer accept()
				if (handleNewConnection(accept(server_listening_socket, NULL, NULL)) == -1)
				{
					std::cerr << "Error: Failed to accept client connection: " << strerror(errno) << std::endl;
					continue;
				}
			}	
			// POLLIN heeft plaatsgevonden en fd komt NIET overeen met server. betekent POLLIN van oude client
			else if ((returned_events & POLLIN) && (fds[i].fd != server_listening_socket))
				checkWhatReceivedFromClient(fds[i].fd);
		}
	}
}

int Server::handleNewConnection(int client_socket)
{
	if (client_socket == -1)
		return (-1);
	std::string username = addClientSocket(client_socket);
	std::cout << "New client connected. ";
	std::cout << "Total clients: " << clients.size() << std::endl;
	sendMotdMessage(client_socket, username); // Toegevoegd want is gangbaar in IRC
	return (0);
}

std::string Server::addClientSocket(int clientSocket)
{
	std::string welcomeMessage;
	const std::string defaultNickname = "Guest";
	std::string randomCode = generateRandomCode();
	std::string username = defaultNickname + randomCode;

	// Check if the client already exists in clients vector
	auto existingClient = std::find_if(clients.begin(), clients.end(), [clientSocket](const Client &user)
									   { return user.getSocketDescriptor() == clientSocket; });

	if (existingClient != clients.end())
	{
		// Client already exists, return the existing usernam
		return existingClient->getNick();
	}

	welcomeMessage = "IRC : To register please use commands PASS - NICK - USER(user, mode, unused, realname)\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	while (isNicknameInUse(username))
	{
		randomCode = generateRandomCode();
		username = defaultNickname + randomCode;
	}

	clients.push_back(Client(clientSocket, username));

	welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + username + " :Welcome to the IRC server, " + username + "!\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	return (username);
}

/*
generateRandomCode() for creating a random code for the "Guest" username.
*/

std::string Server::generateRandomCode()
{
	std::string randomCode;
	srand(time(0)); // Seed for random number generation based on current time
	// Generate a random code with a length of 4 digits
	for (int i = 0; i < 4; ++i)
	{
		char randomDigit = '0' + rand() % 10; // Generate a random digit
		randomCode.push_back(randomDigit);
	}
	return randomCode;
}

/*
isNicknameInUse() checks if a username has already been taken.
*/

int Server::isNicknameInUse(const std::string &nickname) const
{
	for (const auto &Client : clients)
	{
		if (Client.getNick() == nickname)
		{
			return (1);
		}
	}
	return (0);
}

void Server::sendMotdMessage(int client_socket, const std::string &username)
{
	std::string welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + username + " :Welcome to the IRC server, " + username + "!\r\n";
	send(client_socket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
	std::string motdMessage = ":" + std::string(SERVER_NAME) + " 375 " + username + " :- " + std::string(SERVER_NAME) + " Message of the Day -\r\n";
	send(client_socket, motdMessage.c_str(), motdMessage.size(), 0);

	std::string motdContent = ":" + std::string(SERVER_NAME) + " 372 " + username + " :- Welcome to our awesome IRC server! yugioh > magic!!! Enjoy your stay.\r\n";
	send(client_socket, motdContent.c_str(), motdContent.size(), 0);

	std::string endMotdMessage = ":" + std::string(SERVER_NAME) + " 376 " + username + " :End of /MOTD command.\r\n";
	send(client_socket, endMotdMessage.c_str(), endMotdMessage.size(), 0);
}

void Server::checkWhatReceivedFromClient(int client_socket)
{
	std::vector<char> buffer(2048);

	std::string message;
	recv(client_socket, buffer.data(), buffer.size(), 0);
	message.append(buffer.data());
	std::cout << "message size: " << message.size() << std::endl;
	if (message.size() > 0)
	{
		// std::string complete_command(buffer.data(), bytes_received);
		command.processRawClientData(message, \
		getClientByUsername(usernameFromSocket(client_socket)));
	}
	else
	{
		if (message.size() == 0)
		{
			std::cout << "Client disconnected. Total clients: " << clients.size() - 1 << std::endl;
		}
		else
		{
			std::cerr << "Error: Failed to receive data from client: " << strerror(errno) << std::endl;
		}
		close(client_socket);
		auto it = std::remove_if(clients.begin(), clients.end(), [client_socket](const Client &user)
		{ return user.getSocketDescriptor() == client_socket; });
		clients.erase(it, clients.end());
	}
	message.clear();
}

/*
********************************************************************************
Orthodox canonical form
********************************************************************************
*/

Server::Server(int port, const std::string &password) : password(password), active_users(0), port(port), command(clients, channels, *this), server_listening_socket(), IP(), socket_address(), clients(), channels(), welcomeMessage("Welkom bij de yugioh > magic")
{
	// command(clients, channels, *this) = Pass clients and channels vectors and a reference to the current server object
}

Server::~Server()
{
}

/*
********************************************************************************
Getters
********************************************************************************
*/

std::string Server::getPass()
{
	return (this->password);
}

void Server::getHostIP()
{
	char host[256];
	gethostname(host, sizeof(host));

	struct hostent *host_entry;
	host_entry = gethostbyname(host);

	this->IP = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
}

std::string Server::getClientIP(int clientSocket)
{
	for (const auto &user : clients)
	{
		if (user.getSocketDescriptor() == clientSocket)
		{
			return user.getIP();
		}
	}
	return ("Unknown IP"); // Return a default value if client IP is not found
}

int Server::getClientPort(int clientSocket)
{
	for (const auto &user : clients)
	{
		if (user.getSocketDescriptor() == clientSocket)
		{
			return (user.getPort());
		}
	}
	return (0); // IP niet gevonden
}

/*
Returns client username from socket
*/

std::string Server::usernameFromSocket(int clientSocket)
{
	for (const auto &user : clients)
	{
		if (user.getSocketDescriptor() == clientSocket)
		{
			return user.getNick();
		}
	}
	return ("UnknownUsername"); // Username niet gevonden
}

/*
Returns entire client object
*/

Client &Server::getClientByUsername(const std::string &username)
{
	for (auto &user : clients)
	{
		if (user.getNick() == username)
		{
			return (user);
		}
	}
	throw std::runtime_error("Client not found with the specified username");
}

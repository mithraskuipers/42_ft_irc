/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/25 08:01:52 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/28 16:40:43 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>
#include "./../incs/Server.hpp"
#include "./../incs/Command.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/Channel.hpp"
#include <mutex>

std::mutex clientsMutex;

class Server;

void Server::initServer()
{
	int opt = 1;
	this->_serverListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverListeningSocket == 0)
	{
		throw(std::runtime_error("Failed to create socket"));
	}
	int setsockopt_ret = setsockopt(this->_serverListeningSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (setsockopt_ret == -1)
	{
		throw(std::runtime_error("Failed to set socket options"));
	}
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_addr.s_addr = INADDR_ANY;
	this->_socketAddress.sin_port = htons(this->_port);

	if (fcntl(this->_serverListeningSocket, F_SETFL, O_NONBLOCK) < -1)
	{
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	if (bind(this->_serverListeningSocket, (struct sockaddr *)&this->_socketAddress, sizeof(this->_socketAddress)) < 0)
	{
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(this->_serverListeningSocket, QUEUE_SIZE) == -1)
	{
		throw std::runtime_error("Failed to start listening");
	}
	std::cout << "Machine's IP is: " << this->_IP << std::endl;
	std::cout << "Listening on port: " << this->_port << std::endl;
	std::cout << "The password you chose to use is: " << this->_password << std::endl;
}

void Server::startServer()
{
	// Infinite loop to keep the server active
	while (true)
	{
		std::vector<pollfd> fds;

		// Add server socket and client sockets to fds vector
		pollfd serverSocket;
		serverSocket.fd = this->_serverListeningSocket;
		serverSocket.events = POLLIN;
		fds.push_back(serverSocket);

		{
			std::unique_lock<std::mutex> lock(clientsMutex);
			for (const auto &client : this->_clients)
				fds.push_back(client.getPolledFD());
		}

		// Perform poll() on the fds in the vector and check for errors
		if (poll(fds.data(), fds.size(), TIMEOUT_IN_MILLISECONDS) == -1)
		{
			std::cerr << "Error: Failed to perform poll() on socket: " << strerror(errno) << std::endl;
			break;
		}

		// Iterate over readySockets and process events
		for (size_t i = 0; i < fds.size(); ++i)
		{
			int returned_events = fds[i].revents;

			// POLLIN event occurred and fd corresponds to the server socket, indicating a new client connection
			if ((returned_events & POLLIN) && (fds[i].fd == this->_serverListeningSocket))
			{
				// Attempt to accept() the new client connection
				if (handleNewConnection(accept(this->_serverListeningSocket, NULL, NULL)) == -1)
				{
					std::cerr << "Error: Failed to accept client connection: " << strerror(errno) << std::endl;
					continue;
				}
			}
			// POLLIN event occurred and fd does not correspond to the server socket, indicating data from an existing client
			else if ((returned_events & POLLIN) && (fds[i].fd != this->_serverListeningSocket))
			{
				checkWhatReceivedFromClient(fds[i].fd);
			}
		}
	}
}


int Server::handleNewConnection(int clientSocket)
{
	if (clientSocket == -1)
		return (-1);

	std::string clientName = addClientSocket(clientSocket);

	// Check if the channel name is valid
	if (!isValidChannelName(clientName))
	{
		// Invalid channel name, send an error message to the client and close the connection
		std::string errorMessage = "421 " + clientName + " :Invalid channel name\r\n";
		send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
		close(clientSocket);
		// Handle the rest of the processing or return an error code as needed
		return (-1);
	}

	std::cout << "New client connected. ";
	std::cout << "Total clients: " << this->_clients.size() << std::endl;
	sendMotdMessage(clientSocket, clientName); // Send Message of the Day to the client
	// Handle the rest of the processing or return a success code as needed
	return (0);
}

bool Server::isValidChannelName(const std::string &channelName)
{
	// Implement your channel name validation logic here
	// For example, allow only letters and digits in the channel name
	return std::all_of(channelName.begin(), channelName.end(), ::isalnum);
}

std::string Server::addClientSocket(int clientSocket)
{
	std::string welcomeMessage;
	const std::string defaultNickName = "Guest";
	std::string randomCode = generateRandomCode();
	std::string clientName = defaultNickName + randomCode;

	// Check if the client already exists in clients vector
	auto existingClient = std::find_if(this->_clients.begin(), this->_clients.end(), [clientSocket](const Client &client)
									   { return client.getSocketDescriptor() == clientSocket; });

	if (existingClient != this->_clients.end())
	{
		// Client already exists, return the existing clientnam
		return existingClient->getNick();
	}

	welcomeMessage = "IRC : To register please use commands PASS - NICK - USER(client, mode, unused, realName)\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	while (isNickNameInUse(clientName))
	{
		randomCode = generateRandomCode();
		clientName = defaultNickName + randomCode;
	}

	this->_clients.push_back(Client(clientSocket, clientName));

	welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + clientName + " :Welcome to the IRC server, " + clientName + "!\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	return (clientName);
}

/*
generateRandomCode() for creating a random code for the "Guest" clientName.
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
isNickNameInUse() checks if a clientName has already been taken.
*/

int Server::isNickNameInUse(const std::string &nickName) const
{
	for (const auto &Client : this->_clients)
	{
		if (Client.getNick() == nickName)
		{
			return (1);
		}
	}
	return (0);
}

void Server::sendMotdMessage(int clientSocket, const std::string &clientName)
{
	std::string welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + clientName + " :Welcome to the IRC server, " + clientName + "!\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
	std::string motdMessage = ":" + std::string(SERVER_NAME) + " 375 " + clientName + " :- " + std::string(SERVER_NAME) + " Message of the Day -\r\n";
	send(clientSocket, motdMessage.c_str(), motdMessage.size(), 0);

	std::string motdContent = ":" + std::string(SERVER_NAME) + " 372 " + clientName + " :- Welcome to our awesome IRC server! yugioh > magic!!! Enjoy your stay.\r\n";
	send(clientSocket, motdContent.c_str(), motdContent.size(), 0);

	std::string endMotdMessage = ":" + std::string(SERVER_NAME) + " 376 " + clientName + " :End of /MOTD command.\r\n";
	send(clientSocket, endMotdMessage.c_str(), endMotdMessage.size(), 0);
}




void Server::checkWhatReceivedFromClient(int clientSocket)
{
    std::vector<char> buffer(BUFFER_SIZE);
    int bytes_received = recv(clientSocket, buffer.data(), buffer.size(), 0);

    if (bytes_received <= 0)
    {
        std::string clientName = clientNameFromSocket(clientSocket);
        {
            std::unique_lock<std::mutex> lock(clientsMutex);
            std::cout << "Client " << clientName << " disconnected. Total clients: " << this->_clients.size() - 1 << std::endl;

            // Remove client from channels
            for (auto &channel : this->_channels)
            {
                channel.removeClient(&getClientByClientName(clientName));
            }

            close(clientSocket);
            auto it = std::remove_if(this->_clients.begin(), this->_clients.end(), [clientSocket](const Client &client)
                                    { return client.getSocketDescriptor() == clientSocket; });
            this->_clients.erase(it, this->_clients.end());
        }
        return;
    }

    // Handle received data
    std::string receivedData(buffer.data(), bytes_received);
    {
        std::unique_lock<std::mutex> lock(clientsMutex);
        // this->_command.processRawClientData(receivedData, getClientByClientName(clientNameFromSocket(clientSocket)));
		this->_command.processRawClientData(receivedData, &getClientByClientName(clientNameFromSocket(clientSocket)));

    }
}



/*
********************************************************************************
Orthodox canonical form
********************************************************************************
*/

Server::Server(int port, const std::string &password) : _password(password), _activeClients(0), _port(port), _command(_clients, _channels, *this), _serverListeningSocket(), _socketAddress(), _clients(), _channels(), _welcomeMessage("Welkom bij de yugioh > magic")
{
	setMachineIP();
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
	return (this->_password);
}

void Server::setMachineIP()
{
	char host[256];
	gethostname(host, sizeof(host));

	struct hostent *host_entry;
	host_entry = gethostbyname(host);

	this->_IP = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
}

std::string Server::getClientIP(int clientSocket)
{
	for (const auto &client : this->_clients)
	{
		if (client.getSocketDescriptor() == clientSocket)
		{
			return client.getIP();
		}
	}
	return ("Unknown IP"); // Return a default value if client IP is not found
}

int Server::getClientPort(int clientSocket)
{
	for (const auto &client : this->_clients)
	{
		if (client.getSocketDescriptor() == clientSocket)
		{
			return (client.getPort());
		}
	}
	return (0); // IP niet gevonden
}

/*
Returns client clientName from socket
*/

std::string Server::clientNameFromSocket(int clientSocket)
{
	for (const auto &client : this->_clients)
	{
		if (client.getSocketDescriptor() == clientSocket)
		{
			return client.getNick();
		}
	}
	return ("UnknownClientName"); // ClientName niet gevonden
}

/*
Returns entire client object
*/

Client &Server::getClientByClientName(const std::string &clientName)
{
	for (auto &client : this->_clients)
	{
		if (client.getNick() == clientName)
		{
			return (client);
		}
	}
	throw std::runtime_error("Client not found with the specified clientName");
}

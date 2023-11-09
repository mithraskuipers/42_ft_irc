/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 10:34:52 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/07 22:27:15 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Server.hpp"
#include "./../incs/InputParser.hpp"
#include "./../incs/utils.hpp"
#include "./../incs/splash.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

void Server::startServer()
{
	initializeServer();
	showSplash(hostname, port);			   // External
	serverStdout("Server up and running"); // External

	while (1)
	{
		waitForEvents();
		processEvents();
	}
}

void Server::initializeServer()
{
	closeServerSocket();				// Close existing server socket, if any.
	createSocket();						// Create a new socket for the server.
	setSocketOptions();					// Set socket options (e.g., reuse address).
	initServerAddress(std::stoi(port)); // Initialize server address structure with the specified port.
	bindSocket();						// Bind the socket to the server address.
	setNonBlocking();					// Set the socket to non-blocking mode.
	startListening();					// Start listening for incoming connections.
	pollfd server_fd = {this->serverSocket, POLLIN, 0};
	pollfds.push_back(server_fd); // Add server socket to the pollfd list for polling.
}

void Server::closeServerSocket()
{
	if (this->serverSocket >= 0)
	{
		close(this->serverSocket);
		this->serverSocket = -1;
	}
}

void Server::createSocket()
{
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);						// IPv4 address family (AF_INET); stream socket type (SOCK_STREAM); default protocol
	if (serverSocket == -1)
	{
		throw std::runtime_error("Error: socket() failed.");
	}
}

void Server::setSocketOptions()
{
	int opt = 1;
	int setsockopt_ret;
	socklen_t optlen = sizeof(opt);

	setsockopt_ret = setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, optlen); // make options applicable to all sockets (SOL_SOCKET); enable reusing local address and port (SO_REUSEADDR)
	if (setsockopt_ret < 0)
	{
		closeSocketWithMsg(this->serverSocket, "setsockopt()");
	}
}

void Server::initServerAddress(int port)
{
	this->serverAddress.sin_family = AF_INET;									// Set address family to IPv4.
	this->serverAddress.sin_port = htons(port);									// Set port number and convert it to network byte order (big-endian).
	this->serverAddress.sin_addr.s_addr = INADDR_ANY;							// Bind socket to all available network interfaces (INADDR_ANY).
}

void Server::bindSocket()
{
	// Bind server socket to the specified address and port.
	int bind_ret = bind(this->serverSocket,
						(struct sockaddr *)&this->serverAddress,
						sizeof(this->serverAddress));
	if (bind_ret == -1)
	{
		closeSocketWithMsg(this->serverSocket, "bind()");
	}
}

void Server::setNonBlocking()
{
	int fcntl_ret = fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);				// Set the server socket to non-blocking mode.
	if (fcntl_ret == -1)
	{
		closeSocketWithMsg(this->serverSocket, "fcntl()");						// If setting fails, close the socket with an error message.
	}
}

void Server::startListening()
{
	int listen_ret = listen(this->serverSocket, MAX_CONNECTIONS);				// Listen for incoming connections from clients. When listening it can accept new client connections using accept().
	if (listen_ret == -1)
	{
		closeSocketWithMsg(this->serverSocket, "listen()");
	}
	std::cout << "Server is up and running and listening to port " << port << std::endl;
}

void Server::waitForEvents()
{
	int poll_ret;
	
	poll_ret = poll(pollfds.begin().base(), pollfds.size(), -1);
	if (poll_ret < 0)
	{
		throw (std::runtime_error("poll() failed."));
	}
}

void Server::processEvents()
{
	for (pollfds_iterator it = pollfds.begin(); it != pollfds.end(); it++)
	{
		if (it->revents == 0)
		{
			continue;															// No events? Go to the next file descriptor.
		}

		if ((it->revents & POLLHUP) == POLLHUP)									// POLLHUP indicates a hang-up occurred on the associated fd. Often when client disconnected.
		{
			handleClientDisconnect(it->fd);										// Handle client disconnect event.
			break;
		}
		if ((it->revents & POLLIN) == POLLIN)									// POLLIN: Indicates there is data to read on the file descriptor.

		{
			if (it->fd == this->serverSocket)
			{
				handleClientConnection();										// Handle new client connection event.
				break;
			}
			handleClientInput(it->fd);											// Handle client input event.
		}
	}
}

void Server::handleClientDisconnect(int fd)
{
	try
	{
		Client *client;
		std::string message;

		client = this->serverClients.at(fd);
		client->leave();

		message = client->getHostname() + ":" + std::to_string(client->getPort()) + " has disconnected.";
		serverStdout(message);

		this->serverClients.erase(fd);

		for (pollfds_iterator it = pollfds.begin(); it != pollfds.end(); it++)
		{
			if (it->fd != fd)
			{
				continue;
			}
			pollfds.erase(it);
			close(fd);
			break;
		}
		delete (client);
	}
	catch (const std::out_of_range &ex)
	{
	}
}

void Server::handleClientConnection()
{
	int fd;
	int getnameinfo_ret;
	int port;
	sockaddr_in s_address = {};
	socklen_t s_size = sizeof(s_address);

	fd = accept(this->serverSocket, (sockaddr *)&s_address, &s_size);
	if (fd < 0)
	{
		throw std::runtime_error("Failure during accept().");
	}

	pollfd pollfd = {fd, POLLIN, 0};
	pollfds.push_back(pollfd);

	char hostnameBuffer[NI_MAXHOST];											// NI_MAXHOST is max length of hostname
	getnameinfo_ret = getnameinfo((struct sockaddr *)&s_address, sizeof(s_address), hostnameBuffer, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

	if (getnameinfo_ret != 0)
	{
		throw std::runtime_error("Failure during retrieving new client hostname.");
	}

	std::string hostname = std::string(hostnameBuffer);
	port = ntohs(s_address.sin_port);											// Convert network byte order to host byte order, obtaining the port number used by the client.

	Client *client = new Client(fd, hostname, port);							// Create new client object
	this->serverClients.insert(std::make_pair(fd, client));						// Add new client object to serverClients map. Make fd & client object key-value pairs.

	std::string message;
	message = hostname + ":" + std::to_string(port) + " has connected.";
	serverStdout(message);
}

void Server::handleClientInput(int fd)
{
	try
	{
		Client *client = this->serverClients.at(fd);
		std::string message = captureMessage(fd);

		parserObject->invoke(client, message);
	}
	catch (const std::out_of_range &ex)
	{
		
	}
}

std::string Server::captureMessage(int fd)
{
	std::vector<char> buffer(MAX_BUFFER_SIZE);
	std::string message;

	while (1)
	{
		memset(buffer.data(), 0, MAX_BUFFER_SIZE);								// Init memory block -> remove garbage data

		int bytesRead = recv(fd, buffer.data(), MAX_BUFFER_SIZE - 1, 0);		// Receive data from the socket and store the number of bytes read.
		if (bytesRead < 0)
		{
			throw std::runtime_error("Error while reading buffer from client.");
		}
		else if (bytesRead == 0)												// Connection closed by client
		{
			break;
		}
		else
		{
			message.append(buffer.data(), bytesRead);
			if (message.find("\r\n") != std::string::npos)						// \r\n is sequence used by IRC to indicate end of message. npos is returned by find() if not found.
			{
				break;															// Message received completely
			}
		}
	}
	return (message);
}

void Server::closeSocketWithMsg(int socket, const std::string &msg)
{
	perror(msg.c_str());
	close(socket);
	exit(EXIT_FAILURE);
}

/*
Used in JOIN command
*/

Channel *Server::createChannel(const std::string &name, const std::string &password, Client *client)
{

	Channel *channel = new Channel(name, password, client);
	serverChannels.push_back(channel);

	return (channel);
}

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

Server::Server(const std::string &port, const std::string &password) : hostname(HOSTNAME), port(port), password(password)
{
	initializeServer();
	parserObject = new InputParser(this);
}

Server::~Server()
{
	close(this->serverSocket);
}

/*
********************************************************************************
GETTERS / SETTERS
********************************************************************************
*/

Client *Server::getClientInstance(const std::string &nickname)
{
	for (std::map<int, Client *>::iterator it = this->serverClients.begin(); it != this->serverClients.end(); it++)
	{
		if (!nickname.compare(it->second->getNickname()))
			return it->second;
	}
	return (nullptr);
}

Channel *Server::getChannelInstance(const std::string &requestedChannelName)
{

	for (channels_iterator it = serverChannels.begin(); it != serverChannels.end(); it++)
	{
		if (it.operator*()->getChannelName() == requestedChannelName)
		{
			return it.operator*();
		}
	}
	return nullptr;
}

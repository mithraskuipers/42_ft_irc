/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 10:34:52 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:11:04 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Server.hpp"
#include "./../incs/CommandProcessor.hpp"
#include "./../incs/utils.hpp"
#include "./../incs/splash.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

void Server::singleBroadcast(const std::string &firstMessageCombined, std::string include)
{
    for (auto it = serverClients.begin(); it != serverClients.end(); it++)
    {
        if (it->second->getNickname() == include)
        {
            it->second->sendMessageToClientItself(firstMessageCombined);
        }
    }
}

void Server::startServer()
{
	initializeServer();
	showSplash(hostname, port);			   // External
	serverStdout("Server up and running"); // External

	int numEvents;
	
	while (1)
	{
		numEvents = waitForEvents();
		processEvents(numEvents);
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
	// pollfd server_fd = {this->serverSocket, POLLIN, 0};
	// pollfds.push_back(server_fd); // Add server socket to the pollfd list for polling.

	this->epollFD = epoll_create1(0);
	this->singleEventStruct.events = EPOLLIN;
	this->singleEventStruct.data.fd = this->serverSocket;
	epoll_ctl(this->epollFD, EPOLL_CTL_ADD, this->serverSocket, &(this->singleEventStruct));
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
		closeSocketWithMsg(this->serverSocket, "fcntl()");						// If setting fails, close the socket with an error firstMessageCombined.
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

int Server::waitForEvents()
{
	int numEvents;
	numEvents = epoll_wait(this->epollFD, this->multipleEventStruct, MAX_EVENTS, -1);
	if (numEvents < 0)
	{
		throw (std::runtime_error("epoll() failed."));
	}
	return (numEvents);
}

void Server::processEvents(int numEvents)
{
	int i = 0;

	while (i < numEvents)
	{
		int tmpFD = this->multipleEventStruct[i].data.fd;

		if (this->multipleEventStruct[i].events &EPOLLHUP)
			handleClientDisconnect(tmpFD);
		else if (tmpFD == this->serverSocket)
			handleClientConnection();
		else if (this->multipleEventStruct[i].events &EPOLLIN)
			handleClientInput(tmpFD);
		i++;
	}
	// for (pollfds_iterator it = pollfds.begin(); it != pollfds.end(); it++)
	// {
	// 	if (it->revents == 0)
	// 	{
	// 		continue;															// No events? Go to the next file descriptor.
	// 	}

	// 	if ((it->revents & POLLHUP) == POLLHUP)									// POLLHUP indicates a hang-up occurred on the associated fd. Often when client disconnected.
	// 	{
	// 		handleClientDisconnect(it->fd);										// Handle client disconnect event.
	// 		break;
	// 	}
	// 	if ((it->revents & POLLIN) == POLLIN)									// POLLIN: Indicates there is data to read on the file descriptor.

	// 	{
	// 		if (it->fd == this->serverSocket)
	// 		{
	// 			handleClientConnection();										// Handle new client connection event.
	// 			break;
	// 		}
	// 		handleClientInput(it->fd);											// Handle client input event.
	// 	}
	// }
}

void Server::handleClientDisconnect(int fd)
{
	try
	{
		Client *client;
		std::string firstMessageCombined;

		client = this->serverClients.at(fd);
		client->clientLeaveChannel();

		firstMessageCombined = client->getHostname() + ":" + std::to_string(client->getPort()) + " has disconnected.";
		serverStdout(firstMessageCombined);

		this->serverClients.erase(fd);

		// for (pollfds_iterator it = pollfds.begin(); it != pollfds.end(); it++)
		// {
		// 	if (it->fd != fd)
		// 	{
		// 		continue;
		// 	}
		// 	pollfds.erase(it);
			close(fd);
		// 	break;
		// }
		delete (client);
	}
	catch (const std::out_of_range &ex)
	{
	}
}

void Server::handleClientConnection()
{
	int connectSock;
	int getnameinfo_ret;
	int port;
	sockaddr_in s_address = {};
	socklen_t s_size = sizeof(s_address);

	connectSock = accept(this->serverSocket, (sockaddr *)&s_address, &s_size);
	if (connectSock < 0)
	{
		throw std::runtime_error("Failure during accept().");
	}

	// pollfd pollfd = {fd, POLLIN, 0};
	// pollfds.push_back(pollfd);

	fcntl(connectSock, F_SETFL, O_NONBLOCK);
	this->singleEventStruct.events = EPOLLIN | EPOLLET;
	this->singleEventStruct.data.fd = connectSock;
	epoll_ctl(epollFD, EPOLL_CTL_ADD, connectSock, &(this->singleEventStruct));

	char hostnameBuffer[NI_MAXHOST];											// NI_MAXHOST is max length of hostname
	getnameinfo_ret = getnameinfo((struct sockaddr *)&s_address, sizeof(s_address), hostnameBuffer, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

	if (getnameinfo_ret != 0)
	{
		throw std::runtime_error("Failure during retrieving new client hostname.");
	}

	std::string hostname = std::string(hostnameBuffer);
	port = ntohs(s_address.sin_port);											// Convert network byte order to host byte order, obtaining the port number used by the client.

	Client *client = new Client(connectSock, hostname, port, this);							// Create new client object
	this->serverClients.insert(std::make_pair(connectSock, client));						// Add new client object to serverClients map. Make fd & client object key-value pairs.

	std::string firstMessageCombined;
	firstMessageCombined = hostname + ":" + std::to_string(port) + " has connected.";
	serverStdout(firstMessageCombined);
}

void Server::handleClientInput(int fd)
{
	try
	{
		Client *client = this->serverClients.at(fd);
		std::string firstMessageCombined = captureMessage(fd);

		parserObject->process(client, firstMessageCombined);
	}
	catch (const std::out_of_range &ex)
	{
		
	}
}

std::string Server::captureMessage(int fd)
{
	std::vector<char> buffer(MAX_BUFFER_SIZE);
	std::string firstMessageCombined;

	while (1)
	{
		memset(buffer.data(), 0, MAX_BUFFER_SIZE);
		int bytesRead = recv(fd, buffer.data(), MAX_BUFFER_SIZE - 1, 0);		// Receive data from the socket and store the number of bytes read.
		if (bytesRead < 0)
		{
			throw std::runtime_error("Error while reading buffer from client.");
		}
		else if (bytesRead == 0)												// Connection closed by client
		{
			break;
		}
		else // has to differentiate between NC and IRSSI
		{
			// std::string yolo = buffer.data();
			// if (!yolo.find("HELLO"))
			// 	send(fd, buffer.data(), yolo.size(), 0);
			firstMessageCombined.append(buffer.data(), bytesRead);
			if (firstMessageCombined.find("\r\n") != std::string::npos)						// \r\n is sequence used by IRC to indicate end of firstMessageCombined. npos is returned by find() if not found.
			{
				break;															// Message received completely
			}
		}
	}
	return (firstMessageCombined);
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
	parserObject = new CommandProcessor(this);
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
	std::string channelName;
	if (requestedChannelName[0] != '#')
		channelName = "#" + requestedChannelName;
	else 
		channelName = requestedChannelName;
	for (channels_iterator it = serverChannels.begin(); it != serverChannels.end(); it++)
	{
		std::cout << it.operator*()->getChannelName() << std::endl;
		std::cout << channelName << std::endl;
		if (!it.operator*()->getChannelName().compare(channelName))
		{
			return it.operator*();
		}
	}
	return nullptr;
}

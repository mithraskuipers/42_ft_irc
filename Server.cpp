#include "Server.hpp"

void showSplash(const std::string &serverAddress, const std::string &serverPort);

void Server::printServerPrivates() // REMOVE LATER
{
	// /connect 127.0.0.1 6667 pw johnnyBravo
	std::cout << std::endl << "Server: printing: port, pswd, epollfd, servsock" << std::endl;
	std::cout << _port << std::endl;
	std::cout << _password << std::endl;
	std::cout << _epollFD << std::endl;
	std::cout << _serverSocket << std::endl << std::endl;
}

Server::Server(char *port, char *password) : _port(port), _password(password)
{
	// initServer.cpp
	createServerSocket();
	setSocketOptions();
	bindSocketToAddress();
	listenWithSocket();
	monitorSocketEvents(); // monitored with epoll
}

// Main server loop and functions
void Server::runServer()
{
	int i;
	int numEvents;

	std::cout << "Server is up and running and listening to port " << _port << std::endl;
	showSplash(HOSTNAME, _port);
	while (1)
	{
		numEvents = epoll_wait(_epollFD, _tempSavedEvents, MAX_EVENTS, -1);
		if (numEvents == -1)
			throw (std::runtime_error("epoll() failed."));
		i = 0;
		while (i < numEvents)
		{
			if (_tempSavedEvents[i].data.fd == _serverSocket)
				connectNewUser();
			else if (_tempSavedEvents[i].events &EPOLLIN)
				parseInput(_tempSavedEvents[i].data.fd);
			i++;
		}
	}
}

void Server::findCommand(std::string firstMessageCombined)
{
	if (!firstMessageCombined.compare("NICK "))
		std::cout << "FOUND NICK!\n" << firstMessageCombined << std::endl;
}

void Server::parseInput(int eventFD)
{
	std::vector<char> buffer(500);
	std::string firstMessageCombined;

	while (1)
	{
		memset(buffer.data(), 0, 500);
		int bytesRead = recv(eventFD, buffer.data(), 500 - 1, 0);		// Receive data from the socket and store the number of bytes read.
		std::cout << "recv:\n" << buffer.data() << std::endl;
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
			firstMessageCombined.append(buffer.data(), bytesRead);
			if (firstMessageCombined.find("\r\n") != std::string::npos)	
			{
				break;															// Message received completely
			}
		}
	}
	findCommand(firstMessageCombined);
	// std::cout << "reply FD = " << eventFD << std::endl;
	// char buffy[500];
	// recv(eventFD, buffy, sizeof(buffy), 0);
	// std::cout << "new recv\n" << buffy << std::endl;
	// std::string bufstr = buffy;
	// if (!bufstr.find("PING"))
	// {
	// 	std::cout << "pinged" << std::endl;
	// 	send(eventFD, ":johnnyBravo!bde-meij@127.0.0.1 PONG :127.0.0.1\r\n", 48, 0);
	// }
	// else if (!bufstr.find("QUIT"))
	// {
	// 	disconnectUser(eventFD);
	// }
	// else
	// 	std::cout << "no reply" << std::endl;
}

int Server::connectNewUser()
{
	// std::cout << "connect FD = " << eventFD << std::endl;
	sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	int connectSock = accept(_serverSocket, (sockaddr *)&s_address, &s_size);
	if (connectSock < 0)
		throw std::runtime_error("Failure during accept().");

	fcntl(connectSock, F_SETFL, O_NONBLOCK);
	_currentlyHandledEvent.events = EPOLLIN | EPOLLET;
	_currentlyHandledEvent.data.fd = connectSock;
	epoll_ctl(_epollFD, EPOLL_CTL_ADD, connectSock, &(_currentlyHandledEvent));
	std::cout << "connection established" << std::endl;
	std::cout << "connectSock value = " << connectSock << std::endl;
	User joesert(connectSock);
	return (connectSock);
}

void Server::disconnectUser(int fd)
{
	std::cout << "someone disconnected" << std::endl;
	close(fd);
}

void Server::serverStdout(const std::string &firstMessageCombined)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
	std::string str(buffer);
	std::cout << "[" << str << "] " << firstMessageCombined << std::endl;
}

// just canon
// Server::Server()
// {
	
// }

// Server::Server(const Server &Server)
// {

// }

// Server &Server::operator=(const Server &Server)
// {

// }

// Server::~Server()
// {

// }

// initializeServer();
// void Server::initializeServer()
// {

// }

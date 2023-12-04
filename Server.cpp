#include "Server.hpp"
#include <list>
void showSplash(const std::string &serverAddress, const std::string &serverPort);

void Server::printServerPrivates() // REMOVE LATER
{
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
	std::cout << "Server is up and running and listening to port " << _port << std::endl;
	showSplash(HOSTNAME, _port);
}

// Main server loop and functions
void Server::runServer()
{
	int i;
	int numEvents;

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

int Server::connectNewUser()
{
	sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	int connectSock = accept(_serverSocket, (sockaddr *)&s_address, &s_size);
	if (connectSock < 0)
		throw std::runtime_error("Failure during accept().");

	fcntl(connectSock, F_SETFL, O_NONBLOCK);
	_currentlyHandledEvent.events = EPOLLIN;
	_currentlyHandledEvent.data.fd = connectSock;
	epoll_ctl(_epollFD, EPOLL_CTL_ADD, connectSock, &(_currentlyHandledEvent));
	_allUsers.push_back(new User(connectSock));
	return (connectSock);
}

void Server::parseInput(int eventFD)
{
	std::vector<char> buffer(510);
	std::list<std::string> msgList;
	int bytesRead;
	std::string savedMsgParts;
	while (1)
	{
		memset(buffer.data(), 0, 500);
		bytesRead = recv(eventFD, buffer.data(), 500 - 1, 0);		// Receive data from the socket and store the number of bytes read.
		std::cout << "just received:\n" << buffer.data() << " with fd " << eventFD << " and bytes read " << bytesRead << std::endl;
		if (bytesRead < 0)
		{
			throw std::runtime_error("Error while reading buffer from client.");
		}
		else if (bytesRead == 0)												// Connection closed by client
		{
			disconnectUser(eventFD);
			break;
		}
		else
		{
			savedMsgParts.append(buffer.data(), bytesRead);
			// if (savedMsgParts.find("\r\n") != std::string::npos)	// <- \r doesnt work with netcat
			if (savedMsgParts.find("\n") != std::string::npos)	
			{
				break;															// Message received completely
			}
		}
	}
	for (int i = 0; i < bytesRead; i++)
	{
		if (savedMsgParts[i] == '\n')
		{
			msgList.push_back(std::string(savedMsgParts.data(), i));
			savedMsgParts.erase(savedMsgParts.begin(), savedMsgParts.begin() + i + 1);
			bytesRead -= i + 1;
			i = 0;
		}
	}
	for (auto &it : msgList)
		findCommand(it, eventFD);
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

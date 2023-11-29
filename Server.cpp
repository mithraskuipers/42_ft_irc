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
	std::cout << "Server is up and running and listening to port " << _port << std::endl;
}

// Main server loop and functions
void Server::runServer()
{
	showSplash(HOSTNAME, _port);

	while (1)
		processEvents(waitForEvents());
}

int Server::waitForEvents()
{
	int numEvents;
	numEvents = epoll_wait(_epollFD, _tempSavedEvents, MAX_EVENTS, -1);
	if (numEvents == -1)
		throw (std::runtime_error("epoll() failed."));
	return (numEvents);
}

void Server::processEvents(int numEvents)
{
	int i = 0;
	while (i < numEvents)
	{
		int tmpFD = _tempSavedEvents[i].data.fd;
		if (_tempSavedEvents[i].events &EPOLLHUP)
			disconnectUser(tmpFD);
		else if (tmpFD == _serverSocket)
			connectNewUser();
		else if (_tempSavedEvents[i].events &EPOLLIN)
			replyToInput(tmpFD);
		i++;
	}
}

void Server::replyToInput(int tmpFD)
{
	char buffy[500];
	recv(tmpFD, buffy, sizeof(buffy), 0);
	std::cout << buffy << std::endl;
	std::string bufstr = buffy;
	if (!bufstr.find("PING"))
	{
		std::cout << "pinged" << std::endl;
		send(tmpFD, ":johnnyBravo!bde-meij@127.0.0.1 PONG :127.0.0.1\r\n", 48, 0);
	}
	else if (!bufstr.find("QUIT"))
	{
		disconnectUser(tmpFD);
	}
	else if (j == 0)
	{
		std::string jb = "johnnyBravo";
		std::string serv = "serverName";
		std::string vers = "version";
		std::string uM = "userModes";
		std::string cM = "channelModes";

		std::string whatev = RPL_WELCOME(jb) + "\r\n";
		std::cout << whatev << std::endl;
		send(tmpFD, whatev.c_str(), whatev.length(), 0);
		whatev = RPL_YOURHOST(jb, serv, vers) + "\r\n";
		std::cout << whatev << std::endl;
		send(tmpFD, whatev.c_str(), whatev.length(), 0);
		whatev = RPL_CREATED(jb, "date") + "\r\n";
		std::cout << whatev << std::endl;
		send(tmpFD, whatev.c_str(), whatev.length(), 0);
		whatev = RPL_MYINFO(jb, serv, vers, uM, cM) + "\r\n";
		std::cout << whatev << std::endl;
		send(tmpFD, whatev.c_str(), whatev.length(), 0);
		j = 2;
	}
	else
		std::cout << "no reply" << std::endl;
}

void Server::connectNewUser()
{
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

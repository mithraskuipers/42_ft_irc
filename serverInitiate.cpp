#include "Server.hpp"

void Server::checkFailure(int socket, const std::string &msg)
{
	if (_checkFail < 0)
	{
		perror(msg.c_str());
		close(socket);
		exit(EXIT_FAILURE);
	}
}

void Server::createServerSocket()
{
	if (_serverSocket >= 0)
		close(_serverSocket);
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw std::runtime_error("Error: socket() failed.");
}

void Server::setSocketOptions()
{
	int opt = 1;
	socklen_t optlen = sizeof(opt);

	_checkFail = setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, optlen);
	checkFailure(_serverSocket, "setsockopt()");
}

void Server::bindSocketToAddress()
{
	// initializeAdress
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(std::stoi(_port));
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	// bind socket
	_checkFail = bind(_serverSocket, \
	(struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
	checkFailure(_serverSocket, "bind()");
}

void Server::listenWithSocket()
{
	_checkFail = fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
	checkFailure(_serverSocket, "fcntl()");

	_checkFail = listen(_serverSocket, MAX_CONNECTIONS);
	checkFailure(_serverSocket, "listen()");
}

void Server::monitorSocketEvents()
{
	// _epollFD becomes a struct with 2 lists
	// 1 interest list(all added fd)
	// 2 ready list(interests with an event)
	_epollFD = epoll_create1(0);

	// input from users on serversocket will qualify as an event
	_currentlyHandledEvent.events = EPOLLIN;
	_currentlyHandledEvent.data.fd = _serverSocket;

	// if an event is found on serversocket, ADD to epollFD
	epoll_ctl(_epollFD, EPOLL_CTL_ADD, _serverSocket, &(_currentlyHandledEvent));
}

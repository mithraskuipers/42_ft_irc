#include "Server.hpp"

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
	createServerSocket();
	bindSocketToAddress();
	listenWithSocket();
}

// create server socket, set parameters and listen
void Server::createServerSocket()
{
	//closeSocket
	if (_serverSocket >= 0)
	{
		close(_serverSocket);
		_serverSocket = -1;
	}

	//createSocket
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
	{
		throw std::runtime_error("Error: socket() failed.");
	}

	// socketOpts
	int opt = 1;
	int setsockopt_ret;
	socklen_t optlen = sizeof(opt);

	setsockopt_ret = setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, optlen);
	if (setsockopt_ret < 0)
	{
		closeSocketWithMsg(_serverSocket, "setsockopt()");
	}
}

void Server::bindSocketToAddress()
{
	// initializeAdress
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(std::stoi(_port));
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	// bind socket
	int bind_ret = bind(_serverSocket, \
	(struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
	if (bind_ret == -1)
	{
		closeSocketWithMsg(_serverSocket, "bind()");
	}
}

void Server::listenWithSocket()
{
	// set non-blocking
	int fcntl_ret = fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
	if (fcntl_ret == -1)
	{
		closeSocketWithMsg(_serverSocket, "fcntl()");
	}

	//startListen
	int listen_ret = listen(_serverSocket, MAX_CONNECTIONS);
	if (listen_ret == -1)
	{
		closeSocketWithMsg(_serverSocket, "listen()");
	}
	std::cout << "Server is up and running and listening to port " << _port << std::endl;

	// epoll the socket
	_epollFD = epoll_create1(0);
	std::cout << "epoll created: " << _epollFD << std::endl; 
	_currentlyHandledEvent.events = EPOLLIN;
	_currentlyHandledEvent.data.fd = _serverSocket;
	epoll_ctl(_epollFD, EPOLL_CTL_ADD, _serverSocket, &(_currentlyHandledEvent)); //add currentEvent to epollFD-list
}

// Main server loop and functions
void Server::startServerLoop()
{
	showSplash(HOSTNAME, _port);
	int numloops = 0;
	while (1)
	{
		waitForEvents();
		processEvents();
		numloops++;
		if (numloops == 5)
		 break ;
	}
}

void Server::waitForEvents()
{
	std::cout << "waiting on events" << std::endl;
}

void Server::processEvents()
{
	std::cout << "processing" << std::endl;
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

// error handling
void Server::closeSocketWithMsg(int socket, const std::string &msg)
{
	perror(msg.c_str());
	close(socket);
	exit(EXIT_FAILURE);
}
#include "Server.hpp"

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
				recvNextLine(_tempSavedEvents[i].data.fd);
			i++;
		}
	}
}

void Server::recvNextLine(int eventFD)
{
	int byt;
	char buf[BUFFER_SIZE];
	std::list<std::string> msgList;
	User *messenger = findUserByFD(eventFD);

	memset(buf, 0, BUFFER_SIZE);
	byt = recv(eventFD, buf, BUFFER_SIZE, 0);
	if (byt == -1)
		throw std::runtime_error("Error while reading buffer from client.");
	if (byt == 0)
	{
		disconnectUser(eventFD);
		return ;
	}
	
	messenger->addToBuffer(buf);
	msgList = strSplit(messenger->getPersonalBuffer(), '\n');
	if (msgList.size() > 0)
	{
		for (auto &it : msgList)
		{
			std::cout << "\033[1;31m" << "processing:\n" << it << "\033[0m\n" << std::endl;
			findReply(it, eventFD);
		}
		if (findUserByFD(eventFD) != nullptr)
			messenger->clearBuffer();
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

void Server::disconnectUser(int fd)
{
	std::cout << findUserByFD(fd)->getNickName() << " disconnected" << std::endl;
	
	User *tmp = findUserByFD(fd);
	_allUsers.remove(findUserByFD(fd));
	delete(tmp);
	close(fd);
}
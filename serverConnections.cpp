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
		disconnectUser(eventFD);
	
	messenger->addToBuffer(buf);
	msgList = strSplit(messenger->getPersonalBuffer(), '\n');
	if (msgList.size() > 0)
	{
		for (auto &it : msgList)
		{
			std::cout << "\033[1;31m" << "processing:\n" << it << "\033[0m\n" << std::endl;
			findReply(it, eventFD);
		}
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

// void Server::recvNextLine(int eventFD)
// {
// 	std::list<std::string> msgList;
// 	std::string nextLine = "";
// 	char tmpChar[1];
// 	int reading = 1;

// 	while (reading == 1)
// 	{
// 		reading = recv(eventFD, tmpChar, 1, 0);
// 		if (reading < 0)
// 			break ;//throw std::runtime_error("Error while reading buffer from client.");
// 		else if (reading == 0)
// 		{
// 			disconnectUser(eventFD);
// 			break;
// 		}
// 		else if (tmpChar[0] == '\n')
// 		{
// 			msgList.push_back(nextLine);
// 			nextLine.clear();
// 		}
// 		else if (tmpChar[0] != EOF)
// 			nextLine += tmpChar[0];
// 	}

// 	for (auto &it : msgList)
// 	{
// 		std::cout << it << std::endl;
// 		findReply(it, eventFD);
// 	}
// }

// use recv() to read until there is a \n for each line read 
// main reason: avoid errors if we receive partial messages
// void Server::recvNextLine(int eventFD) // TOO MESSY, NEEDS WORK
// {
// 	std::vector<char> buffer(BUFFER_SIZE);
// 	std::list<std::string> msgList;
// 	int bytesRead;
// 	std::string savedMsgParts;
// 	while (1)
// 	{
// 		memset(buffer.data(), 0, BUFFER_SIZE);
// 		bytesRead = recv(eventFD, buffer.data(), BUFFER_SIZE - 1, 0);
// 		std::cout << "\033[1;31m" << "just received:\n" << buffer.data() << "with fd " << eventFD << " and bytes read " << bytesRead << "\033[0m\n" << std::endl; // FOR TESTING RMV LATER
// 		// if (bytesRead < 0)
// 		// {
// 		// 	perror("hrllo: ");
// 		// 	throw std::runtime_error("Error while reading buffer from client.");
// 		// }
// 		// else 
// 		if (bytesRead == 0) // Connection closed by client
// 		{
// 			disconnectUser(eventFD);
// 			break;
// 		}
// 		else
// 		{
// 			savedMsgParts.append(buffer.data(), bytesRead);
// 			// if (savedMsgParts.find("\r\n") != std::string::npos)	// <- \r doesnt work with netcat
// 			if (savedMsgParts.find("\n") != std::string::npos)	
// 			{
// 				break; // Message received completely
// 			}
// 		}
// 	}
// 	for (int i = 0; i < bytesRead; i++)
// 	{
// 		if (savedMsgParts[i] == '\n')
// 		{
// 			msgList.push_back(std::string(savedMsgParts.data(), i));
// 			savedMsgParts.erase(savedMsgParts.begin(), savedMsgParts.begin() + i + 1);
// 			bytesRead -= i + 1;
// 			i = 0;
// 		}
// 	}
// 	// we want to feed findReply one line at a time 
// 	// the msgList iterator is needed if multiple lines were read at once
// 	for (auto &it : msgList)
// 		findReply(it, eventFD);
// }
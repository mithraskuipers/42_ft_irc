#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <algorithm>
#include <vector>
#include <list>
#include <map>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <cstdlib>
#include <ctime>
#include <cstdio>

#include "User.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

#define HOSTNAME "127.0.0.1"
#define BUFFER_SIZE 510
#define MAX_CONNECTIONS 100
#define MAX_EVENTS 100

class Server
{
	public:
	Server(char *port, char *password);

	void addRmvOperator(std::vector<std::string> splitArgs, Channel *channel);
	void setRmvLimit(std::vector<std::string> splitArgs, Channel *channel);
	void setRmvChannelKey(std::vector<std::string> splitArgs, Channel *channel);

	void  runServer();

	void	findReply(std::string fullMsg, int eventFD);
	void	sendReply(int targetFD, std::string msg);
	void	disconnectUser(int eventFD);
	void	recvNextLine(int eventFD);
	int		connectNewUser();

	// serverReply.cpp
	void	rplNick(std::vector<std::string> splitArgs, User *messenger);
	void	rplUser(std::vector<std::string> splitArgs, User *messenger);
	void	rplJoin(std::vector<std::string> splitArgs, User *messenger);
	void	rplPart(std::vector<std::string> splitArgs, User *messenger);
	void	rplPrivmsg(std::vector<std::string> splitArgs, User *messenger);
	void	rplQuit(std::vector<std::string> splitArgs, User *messenger);
	void	rplInvite(std::vector<std::string> splitArgs, User *messenger);
	void	rplKick(std::vector<std::string> splitArgs, User *messenger);
	void	rplMode(std::vector<std::string> splitArgs, User *messenger);
	void	rplTopic(std::vector<std::string> splitArgs, User *messenger);

	// replyUtils.cpp
	int			checkJoinErrors(Channel *channel, User *user, std::vector<std::string> splitArgs);
	void		modeArgsPlus(std::vector<std::string> splitArgs, Channel *channel);
	void		modeArgsMinus(std::vector<std::string> splitArgs, Channel *channel);
	std::string	cleanModes(std::string unclean);
	bool		checkNick(User *messenger, std::string nickname);

	// serverInitiate.cpp
	void	createServerSocket();
	void	setSocketOptions();
	void	bindSocketToAddress();
	void	listenWithSocket();
	void	monitorSocketEvents();
	void	checkFailure(int socket, const std::string &msg);

	// serverUtils.cpp
	std::string	strJoinWithSpaces(std::vector<std::string> splitArgs, size_t startPoint);
	bool		confirmOperator(std::string channelName, User *messenger);
	void		serverStdout(const std::string &firstMessageCombined);
	Channel		*findChannel(std::string channelName);
	User		*findUserByNick(std::string nickName);
	User		*findUserByFD(int fd);

	private:
	char				*_port;
	int					_epollFD;
	std::string			_password;
	std::list<User*>	_allUsers;
	int					_checkFail;
	std::list<Channel*>	_allChannels;
	int					_serverSocket;
	struct sockaddr_in	_serverAddress;
	struct epoll_event	_currentlyHandledEvent;
	struct epoll_event	_tempSavedEvents[MAX_EVENTS];
};

#endif
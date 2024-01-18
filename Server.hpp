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

	// serverInitiate.cpp
	Server(char *port, char *password);

	void 	showSplash(const std::string &serverAddress, const std::string &serverPort);
	void	bindSocketToAddress();
	void	monitorSocketEvents();
	void	createServerSocket();
	void	listenWithSocket();

	// serverConnections.cpp
	void	disconnectUser(int eventFD);
	void	recvNextLine(int eventFD);
	int		connectNewUser();
	void	runServer();

	// serverTools.cpp
	std::string	strJoinWithSpaces(std::vector<std::string> splitArgs, size_t startPoint);
	bool		confirmOperator(std::string channelName, User *messenger);
	std::list<std::string> strSplit(std::string str, char separator);
	void		checkFailure(int socket, const std::string &msg);
	Channel		*findChannel(std::string channelName);
	User		*findUserByNick(std::string nickName);
	User		*findUserByFD(int fd);

	// serverReplies.cpp
	void	rplPrivmsg(std::vector<std::string> splitArgs, User *messenger);
	void	rplNotice(std::vector<std::string> splitArgs, User *messenger);
	void	rplInvite(std::vector<std::string> splitArgs, User *messenger);
	void	rplTopic(std::vector<std::string> splitArgs, User *messenger);
	void	rplNick(std::vector<std::string> splitArgs, User *messenger);
	void	rplUser(std::vector<std::string> splitArgs, User *messenger);
	void	rplJoin(std::vector<std::string> splitArgs, User *messenger);
	void	rplPart(std::vector<std::string> splitArgs, User *messenger);
	void	rplQuit(std::vector<std::string> splitArgs, User *messenger);
	void	rplKick(std::vector<std::string> splitArgs, User *messenger);
	void	rplMode(std::vector<std::string> splitArgs, User *messenger);
	void	rplNetCatter(std::string password, User *messenger);
	void	findReply(std::string fullMsg, int eventFD);
	void	sendReply(int targetFD, std::string msg);

	// replyTools.cpp
	int			checkJoinErrors(Channel *channel, User *user, std::vector<std::string> splitArgs);
	void		modeArgsMinus(std::vector<std::string> splitArgs, Channel *channel);
	void		modeArgsPlus(std::vector<std::string> splitArgs, Channel *channel);
	void		userLeavesChannel(std::string channelName, User *messenger);
	bool		checkNick(User *messenger, std::string nickname);
	void		leaveAllChannels(User *messenger);
	void		shallYouPassWord(User *messenger);
	std::string	cleanModes(std::string unclean);

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
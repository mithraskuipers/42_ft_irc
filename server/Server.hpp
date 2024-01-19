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

#include "../user/User.hpp"
#include "../channel/Channel.hpp"
#include "../replies/Replies.hpp"

#define HOSTNAME "127.0.0.1"
#define BUFFER_SIZE 510
#define MAX_CONNECTIONS 100
#define MAX_EVENTS 100

class Server
{
	public:

	// Server.cpp
	Server(char *port, char *password);
	void	disconnectUser(int eventFD);
	int		connectNewUser();
	void	runServer();

	// recvSend.cpp
	void	recvNextLine(int eventFD);
	void	findReply(std::string fullMsg, int eventFD);
	void	sendReply(int targetFD, std::string msg);

	// serverInitiate.cpp
	void 	showSplash(const std::string &serverAddress, const std::string &serverPort);
	void	checkFailure(int socket, const std::string &msg);
	void	bindSocketToAddress();
	void	monitorSocketEvents();
	void	createServerSocket();
	void	listenWithSocket();

	// serverUtils.cpp
	std::string				strJoinWithSpaces(std::vector<std::string> splitArgs, size_t startPoint);
	std::list<std::string>	strSplit(std::string str, char separator);
	Channel					*findChannel(std::string channelName);
	User					*findUserByNick(std::string nickName);
	User					*findUserByFD(int fd);

	// allReplies.cpp
	void	rplPrivmsg(std::vector<std::string> splitArgs, User *messenger);
	void	rplNotice(std::vector<std::string> splitArgs, User *messenger);
	void	rplKick(std::vector<std::string> splitArgs, User *messenger);
	void		rplMode(std::vector<std::string> splitArgs, User *messenger);
	void	rplNetCatter(std::string password, User *messenger);
	void	rplNick(std::vector<std::string> splitArgs, User *messenger);
	void	rplUser(std::vector<std::string> splitArgs, User *messenger);
	void	rplPart(std::vector<std::string> splitArgs, User *messenger);
	void	rplQuit(std::vector<std::string> splitArgs, User *messenger);
	void	rplInvite(std::vector<std::string> splitArgs, User *messenger);
	void	rplTopic(std::vector<std::string> splitArgs, User *messenger);
	void	rplJoin(std::vector<std::string> splitArgs, User *messenger);

	// replyUtils.cpp
	int			checkJoinErrors(Channel *channel, User *user, std::vector<std::string> splitArgs);
	void		modeArgsMinus(std::vector<std::string> splitArgs, Channel *channel);
	void		modeArgsPlus(std::vector<std::string> splitArgs, Channel *channel);
	void		userLeavesChannel(std::string channelName, User *messenger);
	bool		confirmOperator(std::string channelName, User *messenger);
	bool		checkNick(User *messenger, std::string nickname);
	void		leaveAllChannels(User *messenger);
	std::string	cleanModes(std::string unclean);
	void		checkPassWord(User *messenger);

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
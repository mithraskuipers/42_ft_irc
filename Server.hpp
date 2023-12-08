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

#include <cstdlib>
#include <ctime>
#include <cstdio>

#include "User.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

#define HOSTNAME "127.0.0.1"
#define MAX_CONNECTIONS 100
#define MAX_EVENTS 100

class Server
{
   public:
   Server(char *port, char *password);
   // just canon
   // Server();
   // Server(const Server &Server);
   // Server &operator=(const Server &Server);
   // ~Server();

   void  runServer();

   // serverEvents.cpp
   void  recvNextLine(int eventFD);
   int   connectNewUser();
   void  disconnectUser(int eventFD);

   //serverReply.cpp
   void  computeReply(std::string firstMessageCombined, int eventFD);

   void  rplUser(std::vector<std::string> splitArgs, User *messenger);
   void  rplNick(std::vector<std::string> splitArgs, User *messenger);
   void  rplJoin(std::vector<std::string> splitArgs, User *messenger);
   void  rplPart(std::vector<std::string> splitArgs, User *messenger);
   void  rplPrivmsg(std::vector<std::string> splitArgs, User *messenger);
   void  rplQuit(std::vector<std::string> splitArgs, User *messenger);
   void  rplInvite(std::vector<std::string> splitArgs, User *messenger);
   void  rplKick(std::vector<std::string> splitArgs, User *messenger);
   // void  rplMode(std::vector<std::string> splitArgs, User *messenger);
   void  rplTopic(std::vector<std::string> splitArgs, User *messenger);
   // void  rplPing(User *messenger);
   void  rplWhois(User *messenger);

   void  sendReply(int targetFD, std::string msg);


   // serverInitiate.cpp
	void  createServerSocket();
	void  setSocketOptions();
	void  bindSocketToAddress();
	void  listenWithSocket();
	void  monitorSocketEvents(); // monitored with epoll
   void  checkFailure(int socket, const std::string &msg);

   // serverUtils.cpp
   std::string strJoinWithSpaces(std::vector<std::string> splitArgs, size_t startPoint);
   void  serverStdout(const std::string &firstMessageCombined);
   Channel *findChannel(std::string channelName);
   User *findUserByNick(std::string nickName);
   User *findUserByFD(int fd);
   // void rmvUser(int fd);

   // tmp check
   void printServerPrivates(); // REMOVE LATER

   private:
   std::list<User*> _allUsers;
   std::list<Channel*> _allChannels;
   int   _checkFail;
   char  *_port;
   char  *_password;
   int   _epollFD;
   int   _serverSocket;
   struct sockaddr_in _serverAddress;
   struct epoll_event _currentlyHandledEvent;
	struct epoll_event _tempSavedEvents[MAX_EVENTS];
};

#endif
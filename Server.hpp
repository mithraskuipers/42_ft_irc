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
   void  parseInput(int eventFD);
   void  connectNewUser();
   void  disconnectUser(int eventFD);

   void findCommand(std::string firstMessageCombined, int eventFD);

   // initServer.cpp
	void  createServerSocket();
	void  setSocketOptions();
	void  bindSocketToAddress();
	void  listenWithSocket();
	void  monitorSocketEvents(); // monitored with epoll
   void  checkFailure(int socket, const std::string &msg);

   // util
   void  serverStdout(const std::string &firstMessageCombined);

   // tmp check
   void printServerPrivates(); // REMOVE LATER

   private:
   std::list<User*> _allUsers;
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
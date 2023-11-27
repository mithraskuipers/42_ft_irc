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
#include <map>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <cstdlib>
#include <ctime>
#include <cstdio>

#define HOSTNAME "127.0.0.1"
#define MAX_CONNECTIONS 100
#define MAX_EVENTS 100

class Server
{
   public:
   Server(char *port, char *password);
   void createServerSocket();
	void bindSocketToAddress();
	void listenWithSocket();

   void startServerLoop();
   void waitForEvents();
   void processEvents();

   void closeSocketWithMsg(int socket, const std::string &msg);

   // just canon
   // Server();
   // Server(const Server &Server);
   // Server &operator=(const Server &Server);
   // ~Server();

   private:
   char  *_port;
   char  *_password;
   int   _epollFD;
   int   _serverSocket;
   struct sockaddr_in _serverAddress;
   struct epoll_event _currentlyHandledEvent;
	// struct epoll_event _tempSavedIncomingEvents[MAX_EVENTS];
};

#endif
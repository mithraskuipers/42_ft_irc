/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 11:56:18 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 07:54:24 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

class Server;

#include <algorithm>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <poll.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <sys/epoll.h>

#include "./../incs/CommandProcessor.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/Channel.hpp"

#define MAX_CONNECTIONS 1000
#define HOSTNAME "127.0.0.1" // TODO klopt dit wel voor irc? Moet hostname niet worden uitgelezen uit pc?
#define MAX_BUFFER_SIZE 1024 // Voorkomen van buffer overflows
#define MAX_EVENTS 85

// REGISTRATION LEVELS
#define INIT_CONNECTION_PHASE 0
#define LOGIN_PHASE 1
#define CHATTING_PHASE 2
#define DISCONNECTED 3

class Server
{
	typedef std::vector<pollfd>::iterator pollfds_iterator;
	typedef std::map<int, Client *>::iterator clients_iterator;
	typedef std::vector<Channel *>::iterator channels_iterator;

	int epollFD;
	int serverSocket;
	const std::string hostname;
	const std::string port;
	const std::string password;
	std::vector<pollfd> pollfds;
	std::map<int, Client *> serverClients;										// fd and client instance pairs
	std::vector<Channel *> serverChannels;
	CommandProcessor *parserObject;

	struct epoll_event singleEventStruct;
	struct epoll_event multipleEventStruct[MAX_EVENTS];
	void closeSocketWithMsg(int socket, const std::string &msg);

public:
	Server(const std::string &port, const std::string &password);
	~Server();

	void startServer();
	void prepareServerSocket();
	void processEvents(int numEvents);
	int waitForEvents();
	void initializeServer();
	void closeServerSocket();

	std::string getPassword() const { return password; };
	Client *getClientInstance(const std::string &nickname);

	Channel *getChannelInstance(const std::string &requestedChannelName);
	Channel *createChannel(const std::string &name, const std::string &password, Client *client);

	struct sockaddr_in serverAddress = {};
	void handleClientDisconnect(int fd);
	void handleClientConnection();
	void handleClientInput(int fd);
	std::string captureMessage(int fd);

	// SERVER CONSTRUCTION
	void createSocket();
	void setSocketOptions();
	void initServerAddress(int port);
	void bindSocket();
	void setNonBlocking();
	void startListening();
};

#endif

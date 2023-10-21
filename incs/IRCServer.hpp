#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "channel.hpp"
#include <iostream>
#include <list>
#include <netinet/in.h>
#include "user.hpp"


extern const int MAX_CLIENTS;
extern const int QUEUE_SIZE;

class IRCServer
{
public:
    IRCServer();
    IRCServer(int port, const std::string &password);
    ~IRCServer();

    void initServer();
    void start();

private:
    int active_users;
    int port;
    std::string password;
    int server_listening_socket;
    std::string IP;
    int nConnectedClients = 0;

    struct sockaddr_in socket_address;
    std::list<user> userContainer;
    std::list<channel> channelContainer;

    void getHostIP();
    void addClientSocket(int clientSocket);
    int updateMaxSocketDescriptor();
};

#endif // IRCSERVER_HPP

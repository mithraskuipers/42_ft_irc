#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "../incs/includes.hpp"
#include "../incs/user.hpp"
#include "../incs/channel.hpp"

class channel;

IRCServer::IRCServer() : port(1234), password("")
{
    this->active_users = 0;
    this->getHostIP();
}

IRCServer::~IRCServer() {
    // Perform any necessary cleanup here
}


IRCServer::IRCServer(int port, const std::string &password)
{
    this->active_users = 0;
    this->port = port;
    this->password = password;
    this->getHostIP();
}

void IRCServer::getHostIP()
{
    char host[256];
    gethostname(host, sizeof(host));

    struct hostent *host_entry;
    host_entry = gethostbyname(host);

    this->IP = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
    std::cout << "Host IP is: " << this->IP << std::endl;
}

void IRCServer::initServer()
{
    int opt = 1;
    this->server_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_listening_socket == 0)
    {
        throw std::runtime_error("Failed to create socket");
    }

    int setsockopt_ret = setsockopt(server_listening_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if (setsockopt_ret == -1)
    {
        throw std::runtime_error("Failed to set socket options");
    }

    this->socket_address.sin_family = AF_INET;
    this->socket_address.sin_addr.s_addr = INADDR_ANY;
    this->socket_address.sin_port = htons(port);

    if (fcntl(server_listening_socket, F_SETFL, O_NONBLOCK) < -1)
    {
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    struct sockaddr *socket_address = (struct sockaddr *)&this->socket_address;
    socklen_t address_size = sizeof(this->socket_address);

    if (bind(this->server_listening_socket, socket_address, address_size) < 0)
    {
        throw std::runtime_error("Failed to bind socket");
    }

    std::cout << "Listening on port: " << this->port << std::endl;
    std::cout << "The password you chose to use is: " << this->password << std::endl;

    if (listen(this->server_listening_socket, QUEUE_SIZE) == -1)
    {
        throw std::runtime_error("Failed to start listening");
    }
    std::cout << "Waiting for connection.." << std::endl
              << std::endl;
}

void IRCServer::addClientSocket(int clientSocket)
{
    if (nConnectedClients < MAX_CLIENTS)
    {
        user newUser(clientSocket);
        this->userContainer.push_back(newUser);
        nConnectedClients++;
    }
    else
    {
        // Handle max clients reached case
    }
}

int IRCServer::updateMaxSocketDescriptor()
{
    int maxSocket = server_listening_socket;

    for (const auto &user : userContainer)
    {
        if (user.getSocket() > maxSocket)
        {
            maxSocket = user.getSocket();
        }
    }

    return maxSocket;
}

void IRCServer::start()
{
    fd_set fd_pack;
    char buffer[9999];

    while (true)
    {
        FD_ZERO(&fd_pack);
        FD_SET(this->server_listening_socket, &fd_pack);

        int max_socket_fd = updateMaxSocketDescriptor();

        for (const auto &user : userContainer)
        {
            FD_SET(user.getSocket(), &fd_pack);
        }

        int readySockets = select(max_socket_fd + 1, &fd_pack, NULL, NULL, NULL);

        if (readySockets == -1)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(this->server_listening_socket, &fd_pack))
        {
            int newClientSocket = accept(this->server_listening_socket, NULL, NULL);
            if (newClientSocket == -1)
            {
                perror("accept");
            }
            else
            {
                this->addClientSocket(newClientSocket);
                std::cout << "Client connected. Total users: " << nConnectedClients << std::endl;
            }
        }

        for (auto it = userContainer.begin(); it != userContainer.end();)
        {
            auto &user = *it;
            if (FD_ISSET(user.getSocket(), &fd_pack))
            {
                int bytes_received = recv(user.getSocket(), buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    // Handle disconnect or error
                    close(user.getSocket());
                    std::cout << "Client disconnected. Total users: " << nConnectedClients << std::endl;
                    it = userContainer.erase(it);
                }
                else
                {
                    // Handle received data
                    std::string receivedData(buffer, bytes_received);
                    user.commandHandler(receivedData, this->channelContainer);
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }
    }
}

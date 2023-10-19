/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   User.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/17 22:22:03 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/18 23:01:05 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/User.hpp"

#include <arpa/inet.h>   // For inet_ntop
#include <netinet/in.h>  // For sockaddr_in
#include <sys/socket.h>  // For getpeername
#include <unistd.h>      // For close function

// Constructor
User::User(int socket_descriptor, const std::string& nick) : socket_descriptor(socket_descriptor), port(0), _nickname(nick)
{
}

// Getter function for the socket descriptor
int User::getSocketDescriptor() const
{
	return socket_descriptor;
}

// Getter function for the buffer
std::string& User::getBuff()
{
	return buff;
}

// Getter function for the User's IP address
std::string User::getIP() const
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char ip[INET_ADDRSTRLEN];
    
    if (getpeername(socket_descriptor, (struct sockaddr*)&addr, &addr_len) == 0)
	{
        inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
        return ip;
    }
	else
	{
        return "Unknown IP";
    }
}

bool User::getRegisteredStatus() const
{
    return registered;
}

// Setter function to set the User's IP address
void User::setIP(const std::string& clientIP)
{
	ip = clientIP;
}

// Getter function for the User's port
int User::getPort() const
{
	return port;
}

// Setter function to set the User's port
void User::setPort(int clientPort)
{
	port = clientPort;
}

// Getter function for the User's nickname
std::string User::getNick() const
{
	return _nickname;
}

User &User::operator=(const User &src)
{
    if (this == &src)
        return *this;
    this->_nickname = src._nickname;
    this->_password = src._password;
    return *this;
}

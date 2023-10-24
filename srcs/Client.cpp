/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/17 22:22:03 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/24 19:37:14 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Client.hpp"
#include <iostream>


#include <arpa/inet.h>	// For inet_ntop
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For getpeername
#include <unistd.h>		// For close function

// Constructor
// Other necessary includes

Client::Client(int socket_descriptor, const std::string &nick) : socket_descriptor(socket_descriptor), port(0), _nickname(nick), registered(false), ip("Unknown IP")
{
	// Constructor implementation
}

// Getter function for the socket descriptor
int Client::getSocketDescriptor() const
{
	return (socket_descriptor);
}

// Getter function for the buffer
std::string &Client::getBuff()
{
	return (buff);
}


void Client::setNick(const std::string &newNick)
{
	_nickname = newNick;
}

void Client::setRealName(const std::string &newRealName)
{
	_realname = newRealName;
}

void Client::send(const std::string &data)
{
	ssize_t bytesSent = write(socket_descriptor, data.c_str(), data.size());

	if (bytesSent < 0)
	{
		// Handle write error
	}
}

void Client::setRegistered(bool value)
{
	registered = value;
}

void Client::sendToClient(const std::string &message)
{
	::send(this->getSocketDescriptor(), message.c_str(), message.length(), 0);
}

// Getter function for the Client's IP address
std::string Client::getIP() const
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	char ip[INET_ADDRSTRLEN];

	if (getpeername(socket_descriptor, (struct sockaddr *)&addr, &addr_len) == 0)
	{
		inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
		return ip;
	}
	else
	{
		return ("Unknown IP");
	}
}

bool Client::getRegisteredStatus() const
{
	return registered;
}

std::string Client::getRealName() const
{
	return (_realname);
}

// Setter function to set the Client's IP address
void Client::setIP(const std::string &clientIP)
{
	ip = (clientIP);
}

// Getter function for the Client's port
int Client::getPort() const
{
	return (port);
}

// Setter function to set the Client's port
void Client::setPort(int clientPort)
{
	this->port = clientPort;
}

// Getter function for the Client's nickname
std::string Client::getNick() const
{
	return (_nickname);
}

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

// Constructor
User::User(int socket_descriptor, const std::string& nick) : socket_descriptor(socket_descriptor), port(0), nickname(nick)
{
	
} // Modified constructor to accept nickname

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

// Getter function for the client's IP address
std::string User::getIP() const
{
	return ip;
}

bool User::getRegisteredStatus() const
{
    return registered;
}

// Setter function to set the client's IP address
void User::setIP(const std::string& clientIP)
{
	ip = clientIP;
}

// Getter function for the client's port
int User::getPort() const
{
	return port;
}

// Setter function to set the client's port
void User::setPort(int clientPort)
{
	port = clientPort;
}

// Getter function for the client's nickname
std::string User::getNick() const
{
	return nickname;
}

User &User::operator=(const User &src)
{
	if (this == &src)
		return *this;
	this->_username = src._username;
    this->_password = src._password;
	return *this;
}
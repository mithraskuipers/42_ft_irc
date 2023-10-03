/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/01 17:56:34 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/09/06 22:50:26 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class user;

class IRCServer
{
	public:
		IRCServer(); // default constructor
		IRCServer(int port, const std::string &password); // parameterized constructor
		~IRCServer(); // destructor
		void	initServer(); // initServer server
		void	start(); // start server
        void    getHostIP(); // get host ip
		std::string getIP() const;
		int 	getPort() const;
		int 	updateMaxSocketDescriptor();
		void	addClientSocket(int clientSocket);
	private:
		unsigned int active_users;
		user		*userArray;
		channel		*channelArray;
		int port;
        int server_listening_socket;
		std::string password;
        std::string IP;
        struct sockaddr_in socket_address;
		int nConnectedClients = 0;
		int client_socket_array[MAX_CLIENTS];

};

#endif
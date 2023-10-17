/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/01 17:56:34 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/10/17 22:37:18 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in
#include <list>

class IRCServer
{
	public:
		IRCServer();
		IRCServer(int port, const std::string &password);
		~IRCServer();
		void initServer(); // initialize server
		void start(); // start server
		void getHostIP(); // get host ip
		std::string getIP() const;
		int getPort() const;
		void addClientSocket(int clientSocket);
		int updateMaxSocketDescriptor();
	private:
		int port;
		int server_listening_socket;
		std::string password;
		std::string IP;
		struct sockaddr_in socket_address;
		std::list<Client> clients;
		int nConnectedClients;
};

#endif
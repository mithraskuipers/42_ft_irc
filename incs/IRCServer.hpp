#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in


class IRCServer
{
	public:
		IRCServer(); // default constructor
		IRCServer(int port, const std::string &password); // parameterized constructor
		~IRCServer(); // destructor
		void	initServer(); // initServer server
		void	start(); // start server
        void    getHostIP(); // get host ip
	private:
		int port;
        int main_socket;
		std::string password;
        std::string IP;
        struct sockaddr_in socket_address;

};

#endif
#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "includes.hpp"

class IRCServer
{
	public:
		IRCServer(); // default constructor
		IRCServer(int port, const std::string &password); // parameterized constructor
		~IRCServer();
		void	init();
		void	start();
        void    getHostname();
	private:
		int port;
		std::string password;

};

#endif
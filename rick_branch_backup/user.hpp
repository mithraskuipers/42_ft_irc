#ifndef USER_HPP
# define USER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class channel;

class user
{
	public:
		user(); // default constructor
		user(std::string username, std::string password); // parameterized constructor
		~user(); // destructor

		user &operator=(const user &src);
		void commandHandler(std::string buffer, user commandGiver, channel *channelArray);
		void setPass(std::string);
		void setName(std::string);
		std::string getName();
		std::string getPass();
		int getSocket();
		void setSocket(int socket_fd);
		int getid();
		void setid(int id);

	private:
        std::string _username;
        std::string _password;
		int 		_socket;
		int			_id;
        // int         _channel;
        // int         _permissions;

};

#endif
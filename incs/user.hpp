#ifndef USER_HPP
# define USER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class user
{
	public:
		user(); // default constructor
		user(std::string username, std::string password); // parameterized constructor
		~user(); // destructor

		user &operator=(const user &src);
		void setPass(std::string);
		void setName(std::string);
		std::string getName();
		std::string getPass();

	private:
        std::string _username;
        std::string _password;
        // int         _channel;
        // int         _permissions;

};

#endif
#include "../incs/user.hpp"

user::user()
{
    
}

user::user(std::string username, std::string password) : _username(username), _password(password)
{

}

user::~user()
{

}


user &user::operator=(const user &src)
{
	if (this == &src)
		return *this;
	this->_username = src._username;
    this->_password = src._password;
	return *this;
}

void user::setName(std::string username)
{
    this->_username = username;
}

void user::setPass(std::string password)
{
    this->_password = password;
}

std::string user::getName()
{
    return this->_username;
}

std::string user::getPass()
{
    return this->_password;
}
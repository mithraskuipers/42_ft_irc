#include "../incs/includes.hpp"
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

void    user::commandHandler(std::string buffer, user commandGiver, channel *channelArray)
{
    int i = 0;
    std::string command;

    command = buffer.substr(1, buffer.find(" "));
    std::cout << "command: " << command << std::endl;
    if (command == "JOIN")
    {
        command = buffer.substr(buffer.find(" ") + 1, buffer.find("\n"));
        while (channelArray[i].getName() != "")
        {
            if (channelArray[i].getName() == command)
            {
                channelArray[i].addUser(commandGiver);
                return;
            }
            i++;
        }
        channelArray[i].setName(command);
        channelArray[i].addUser(commandGiver);
        return;
    }
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

int user::getSocket()
{
    return(this->_socket);
}
void user::setSocket(int socket_fd)
{
    this->_socket = socket_fd;
}

int user::getid()
{
    return(this->_id);
}
void user::setid(int id)
{
    this->_id = id;
}

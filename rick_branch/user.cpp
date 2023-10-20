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
    std::string command;

    std::cout << "Sockettttttt: " <<commandGiver.getSocket() << std::endl;

    command = buffer.substr(1, buffer.find(" ") - 1);
    std::cout << "command: " << command << std::endl;
    if (strcmp(command.c_str(), "JOIN") == 0)
    {
        int i = 0;
        command = buffer.substr(buffer.find(" ") + 1, buffer.find("\n") - 1);
        while (channelArray[i].getName() != "")
        {
             std::cout << "Command: " << command << std::endl;
              std::cout << "Channel name: " << channelArray[i].getName() << std::endl;
            if (channelArray[i].getName().compare(command))
            {
                channelArray[i].addUser(commandGiver);
                return;
            }
            i++;
        }
        return;
    }

    if (strcmp(command.c_str(), "CREATE") == 0)
    {
        int i = 0;
        command = buffer.substr(buffer.find(" ") + 1, buffer.find("\n") - 1);
        while (channelArray[i].getName() != "")
        {
            if (channelArray[i].getName().compare(command))
            {
                std::cout << "channel already exists" << std::endl;
                return;
            }
            std::cout << i << std::endl;
            i++;
        }
        channelArray[i].setName(command);
        channelArray[i].addUser(commandGiver);
        std::cout << "channel '" << command << "' created" << std::endl;
        return;
    }

    if (strcmp(command.c_str(), "LIST") == 0)
    {
        int i = 0;
        std::string message = "Channels: ";
        while (channelArray[i].getName() != "")
        {
            message += channelArray[i].getName() + " ";
            i++;
        }
        message += "\n";
        send(commandGiver.getSocket(), message.c_str(), strlen(message.c_str()), 0);
        return;
    }
    else 
        std::cout << "command not found: " << command << std::endl;
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

#include "./../incs/channel.hpp"

channel::channel()
{
    
}

channel::channel(std::string channelName) : _channelName(channelName)
{
    this->_channelMembers = new user[100];
}

channel::~channel()
{

}


channel &channel::operator=(const channel &src)
{
	std::cout << "channel Assignation operator called" << std::endl;
	if (this == &src)
		return *this;
	this->_channelName = src._channelName;
	return *this;
}

void channel::addUser(user newUser)
{
    this->_channelMembers[this->_userCount] = newUser;
    this->_userCount++;
    std::cout << newUser.getName() << " added to channel: " << this->_channelName << std::endl;
}

void channel::removeUser(user oldUser)
{
    int i = 0;
    while (i < this->_userCount)
    {
        if (this->_channelMembers[i].getName() == oldUser.getName())
        {
            while (i < this->_userCount)
            {
                this->_channelMembers[i] = this->_channelMembers[i + 1];
                i++;
            }
            this->_userCount--;
            return;
        }
        i++;
    }
}
#include "Channel.hpp"
#include "Server.hpp"

void Channel::printChannelPrivates() // REMOVE LATER
{
	std::cout << std::endl << "Channel: printing: channelName, isInviteOnly" << std::endl;
	std::cout << _channelName << std::endl;
	std::cout << _isInviteOnly << std::endl << std::endl;
}

Channel::Channel(std::string channelName) : _topic(""), _channelName(channelName), _isInviteOnly(0)
{

}

void Channel::msgAllInChannel(std::string msg)
{
	size_t i = 0;
	while (i < _joinedUserFDs.size())
	{
		send(_joinedUserFDs[i], msg.c_str(), msg.length(), 0);
		i++;
	}
}

void Channel::addToChannel(int userFD)
{
	size_t i = 0;
	while (i < _joinedUserFDs.size())
	{
		if (_joinedUserFDs[i] == userFD)
		{
			std::cout << "user already in room" << std::endl;
			return ;
		}
		i++;
	}
	_joinedUserFDs.push_back(userFD);
}

void Channel::rmvFromChannel(int userFD)
{
	size_t i = 0;
	while (i < _joinedUserFDs.size())
	{
		if (_joinedUserFDs[i] == userFD)
			_joinedUserFDs.erase(_joinedUserFDs.begin() + i);
		i++;
	}
}

void Channel::addToBanned(int userFD)
{
	size_t i = 0;
	while (i < _bannedUserFDs.size())
	{
		if (_bannedUserFDs[i] == userFD)
		{
			std::cout << "user already banned" << std::endl;
			return ;
		}
		i++;
	}
	_bannedUserFDs.push_back(userFD);
}

void Channel::rmvFromBanned(int userFD)
{
	size_t i = 0;
	while (i < _bannedUserFDs.size())
	{
		if (_bannedUserFDs[i] == userFD)
			_bannedUserFDs.erase(_bannedUserFDs.begin() + i);
		i++;
	}
}


bool Channel::isInChannel(int userFD)
{
	size_t i = 0;
	while (i < _joinedUserFDs.size())
	{
		if (_joinedUserFDs[i] == userFD)
			return (1);
		i++;
	}
	return (0);
}

bool Channel::isBanned(int userFD)
{
	size_t i = 0;
	while (i < _bannedUserFDs.size())
	{
		if (_bannedUserFDs[i] == userFD)
			return (1);
		i++;
	}
	return (0);
}

std::string Channel::getTopic()
{
	return (_topic);
}

std::string Channel::getChannelName()
{
	return (_channelName);
}

bool        Channel::getIsInviteOnly()
{
	return (_isInviteOnly);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setChannelName(std::string channelName)
{
	_channelName = channelName;
}

void Channel::setIsInviteOnly(bool inviteBool)
{
	_isInviteOnly = inviteBool;
}


// Channel::Channel()
// {
	
// }

// Channel::Channel(const Channel &Channel)
// {

// }

// Channel &Channel::operator=(const Channel &Channel)
// {

// }

// Channel::~Channel()
// {

// }
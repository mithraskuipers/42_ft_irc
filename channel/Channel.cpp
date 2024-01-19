#include "Channel.hpp"
#include "../server/Server.hpp"

Channel::Channel(std::string channelName) : _topic(""), _channelName(channelName), _channelKey(""), _activeModes(""), _limit(-1)
{
	std::cout << "channel " << channelName << " created" << std::endl;
	int i = 0;
	while (i < 4)
	{
		_intArr[i] = 0;
		i++;
	}
}

Channel::~Channel()
{
	std::cout << "channel " << _channelName << " destroyed" << std::endl;
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
			return ;
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

void Channel::addToOperators(int userFD)
{
	size_t i = 0;
	while (i < _operatorFDs.size())
	{
		if (_operatorFDs[i] == userFD)
			return ;
		i++;
	}
	_operatorFDs.push_back(userFD);
}

void Channel::rmvFromOperators(int userFD)
{
	size_t i = 0;
	while (i < _operatorFDs.size())
	{
		if (_operatorFDs[i] == userFD)
			_operatorFDs.erase(_operatorFDs.begin() + i);
		i++;
	}
}

void Channel::addToBanned(int userFD)
{
	size_t i = 0;
	while (i < _bannedUserFDs.size())
	{
		if (_bannedUserFDs[i] == userFD)
			return ;
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

bool Channel::isOperator(int userFD)
{
	size_t i = 0;
	while (i < _operatorFDs.size())
	{
		if (_operatorFDs[i] == userFD)
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

int Channel::getLimit()
{
	return (_limit);
}

std::string Channel::getTopic()
{
	return (_topic);
}

std::string Channel::getChannelName()
{
	return (_channelName);
}

std::string Channel::getChannelKey()
{
	return (_channelKey);
}

std::string Channel::getActiveModes()
{
	return (_activeModes);
}

int Channel::getNumOfUsers()
{
	return ((int) _joinedUserFDs.size());
}

int Channel::getNumOfOperators()
{
	return ((int) _operatorFDs.size());
}

int Channel::getFirstJoinedUserFD()
{
	return(_joinedUserFDs[0]);
}

void Channel::setLimit(int limit)
{
	_limit = limit;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setChannelName(std::string channelName)
{
	_channelName = channelName;
}

void Channel::setChannelKey(std::string channelKey)
{
	_channelKey = channelKey;
}

void Channel::setActiveModes(std::string modes)
{
	size_t i = 0;
	int onOff = 1;
	_activeModes = modes[0];
	std::string optionArr = "itkl";

	if (modes[0] == '-')
		onOff = 0;
	while (i < modes.size())
	{
		if (modes[i] == 'i')
			_intArr[0] = onOff;
		if (modes[i] == 't')
			_intArr[1] = onOff;
		if (modes[i] == 'k')
			_intArr[2] = onOff;
		if (modes[i] == 'l')
			_intArr[3] = onOff;
		i++;
	}
	i = 0;
	while (i < 4)
	{
		if (_intArr[i] == 1)
			_activeModes += optionArr[i];
		i++;
	}
}

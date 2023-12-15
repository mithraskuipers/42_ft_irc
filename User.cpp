#include "User.hpp"

void User::printUserPrivates() // REMOVE LATER
{
	std::cout << std::endl << "User: printing: " << std::endl;
	std::cout << "fd " << _userFD << std::endl;
	std::cout << "nick " << _nickName << std::endl;
	std::cout << "useN " << _userName << std::endl;
	std::cout << "host " << _hostName << std::endl;
	std::cout << "real " << _realName << std::endl;
	std::cout << "source " << getSource() << std::endl;
}

// User::User(int fd, std::string userName, std::string nickName, std::string realName, bool isOperator) :
// _userFD(fd), _userName(userName), _nickName(nickName), _realName(realName), _isOperator(isOperator)
User::User(int fd) : _userFD(fd), _nickName(""), _userName(""), _hostName(""), _realName("") //, _isOperator(0)
{
	std::cout << "\033[1;33m" << "_userFD set to " << _userFD << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

std::string User::getSource()
{
	return (_nickName + "!" + _userName + "@" + _hostName);
}

void User::addInvitation(std::string channelName)
{
	_channelInvitations.push_back(channelName);
}

bool User::isInvited(std::string channelName)
{
	for (auto const &i : _channelInvitations)
	{
		if (!i.compare(channelName))
			return (1);
	}
	return (0);
}

void User::addJoinedChannel(std::string channelName)
{
	_joinedChannels.push_back(channelName);
}

void User::rmvJoinedChannel(std::string channelName)
{
	size_t i = 0;
	while (i < _joinedChannels.size())
	{
		if (!_joinedChannels[i].compare(channelName))
			_joinedChannels.erase(_joinedChannels.begin() + i);
		i++;
	}
}

bool User::isInChannel(std::string channelName)
{
	for (auto const &i : _joinedChannels)
	{
		if (!i.compare(channelName))
			return (1);
	}
	return (0);
}

void User::setUserFD(int fd)
{
	_userFD = fd;
	std::cout << "\033[1;33m" << "_userFD set to " << _userFD << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setNickName(std::string nickName)
{
	_nickName = nickName;
	std::cout << "\033[1;33m" << "_nickName set to " << _nickName << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setUserName(std::string userName)
{
	_userName = userName;
	std::cout << "\033[1;33m" << "_userName set to " << _userName << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setHostName(std::string hostName)
{
	_hostName = hostName;
	std::cout << "\033[1;33m" << "_hostName set to " << _hostName << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setRealName(std::string realName)
{
	// realName.erase(0); // this line deletes the ':' from ":real name", but in most cases it is printed anyway
	_realName = realName;
	std::cout << "\033[1;33m" << "_realName set to " << _realName << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setPassword(std::string password)
{
	// realName.erase(0); // this line deletes the ':' from ":real name", but in most cases it is printed anyway
	_password = password;
	std::cout << "\033[1;33m" << "_password set to " << _password << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

int User::getUserFD()
{
	return (_userFD);
}

std::string User::getNickName()
{
	return (_nickName);
}

std::string User::getUserName()
{
	return (_userName);
}

std::string User::getHostName()
{
	return(_hostName);
}

std::string User::getRealName()
{
	return (_realName);
}

std::string User::getPassword()
{
	return (_password);
}

// bool User::findChannel(std::string channelToFind)
// {
	// loop through joinedChannels, compare to argument
	// return 1 if found
	// return 0 if not found
// }

// bool User::findChannelInvitation(std::string channelToFind)
// {
	// loop through channels, compare to argument
	// return 1 if found
	// return 0 if not found
// }

// canon
// User::User()
// {
	
// }

// User::User(const User &User)
// {

// }

// User &User::operator=(const User &User)
// {

// }

// User::~User()
// {

// }
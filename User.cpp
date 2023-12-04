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
	std::cout << "isOp " << _isOperator << std::endl << std::endl;
}

// User::User(int fd, std::string userName, std::string nickName, std::string realName, bool isOperator) :
// _userFD(fd), _userName(userName), _nickName(nickName), _realName(realName), _isOperator(isOperator)
User::User(int fd) : _userFD(fd), _nickName(""), _userName(""), _hostName(""), _realName(""), _isOperator(0)
{
	
}

std::string User::getSource()
{
	return (_nickName + "!" + _userName + "@" + _hostName);
}

void User::setUserFD(int fd)
{
	_userFD = fd;
}

void User::setNickName(std::string nickName)
{
	std::cout << "nickname is set" << std::endl;
	_nickName = nickName;
}

void User::setUserName(std::string userName)
{
	_userName = userName;
}

void User::setHostName(std::string hostName)
{
	std::cout << "hostname is set" << std::endl;
	_hostName = hostName;
}

void User::setRealName(std::vector<std::string> splitArgs)
{
	size_t i = 4;
	splitArgs[4].erase(0, 1);
	while (i < splitArgs.size())
	{
		_realName += splitArgs[i];
		if (i != splitArgs.size())
			_realName += " ";
		i++;
	}
}

void User::setIsOperator(bool isOperator)
{
	_isOperator = isOperator;
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

bool User::getIsOperator()
{
	return (_isOperator);
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
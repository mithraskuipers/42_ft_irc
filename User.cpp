#include "User.hpp"

void User::printUserPrivates() // REMOVE LATER
{
	std::cout << std::endl << "User: printing: usrFD, name, nick, realname, host, isOp" << std::endl;
	std::cout << _userFD << std::endl;
	std::cout << _userName << std::endl;
	std::cout << _nickName << std::endl;
	std::cout << _realName << std::endl;
	std::cout << _hostName << std::endl;
	std::cout << _isOperator << std::endl << std::endl;
}

// User::User(int fd, std::string userName, std::string nickName, std::string realName, bool isOperator) :
// _userFD(fd), _userName(userName), _nickName(nickName), _realName(realName), _isOperator(isOperator)
User::User(int fd) : _userFD(fd), _userName(""), _nickName(""), _realName(""), _isOperator(0)
{
	
}

void User::setNickName(std::string nickName)
{
	_nickName = nickName;
}

void User::setUserFD(int FD)
{
	_userFD = FD;
}

int User::getUserFD()
{
	return (_userFD);
}

void User::setUserName(std::string userName)
{
	_userName = userName;
}

void User::setHostName(std::string hostName)
{
	_hostName = hostName;
}

std::string User::getUserName()
{
	return (_userName);
}

std::string User::getNickName()
{
	return (_nickName);
}

void User::setRealName(std::string realName)
{
	_realName = realName;
}

std::string User::getRealName()
{
	return (_realName);
}

void User::setIsOperator(bool isOperator)
{
	_isOperator = isOperator;
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
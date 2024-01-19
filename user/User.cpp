#include "User.hpp"

User::User(int fd) : _userFD(fd), _nickName(""), _userName(""), _hostName(""), _realName(""), _personalBuffer("")
{

}
User::~User()
{
	std::cout << _nickName << " was destroyed" << std::endl;
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

bool User::isIncompleteUser()
{
	if (_nickName.empty() || _userName.empty() || \
	_hostName.empty() || _realName.empty() || _password.empty())
		return (1);
	return (0);
}

void User::setUserFD(int fd)
{
	_userFD = fd;
}

void User::setNickName(std::string nickName)
{
	_nickName = nickName;
}

void User::setUserName(std::string userName)
{
	_userName = userName;
}

void User::setHostName(std::string hostName)
{
	_hostName = hostName;
}

void User::setRealName(std::string realName)
{
	_realName = realName;
}

void User::setPassword(std::string password)
{
	_password = password;
}

void User::addToBuffer(std::string msgPart)
{
	_personalBuffer += msgPart;
}

void User::clearBuffer()
{
	_personalBuffer.clear();
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

std::string User::getPersonalBuffer()
{
	return (_personalBuffer);
}

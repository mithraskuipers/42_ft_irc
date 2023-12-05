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
	std::cout << "\033[1;33m" << "_userFD set to " << _userFD << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

std::string User::getSource()
{
	return (_nickName + "!" + _userName + "@" + _hostName);
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

void User::setRealName(std::vector<std::string> splitArgs)
{
	size_t i = 4;
	// splitArgs[4].erase(0, 1); // this line deletes the ':' from ":real name", but in most cases it is printed anyway
	while (i < splitArgs.size())
	{
		_realName += splitArgs[i];
		if (i != splitArgs.size())
			_realName += " ";
		i++;
	}
	std::cout << "\033[1;33m" << "_realName set to " << _realName << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
}

void User::setIsOperator(bool isOperator)
{
	std::cout << "\033[1;33m" << "isOperator set to " << isOperator << "\033[0m" << std::endl; // FOR TESTING, RMV LATER
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
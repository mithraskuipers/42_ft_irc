#include "User.hpp"

void User::printUserPrivates() // REMOVE LATER
{
	std::cout << std::endl << "User: printing: usrFD, name, nick, realname, isOp" << std::endl;
	std::cout << _userFD << std::endl;
	std::cout << _userName << std::endl;
	std::cout << _nickName << std::endl;
	std::cout << _realName << std::endl;
	std::cout << _isOperator << std::endl << std::endl;
}

// User::User(int fd, std::string userName, std::string nickName, std::string realName, bool isOperator) :
// _userFD(fd), _userName(userName), _nickName(nickName), _realName(realName), _isOperator(isOperator)
User::User(int fd) : _userFD(fd), _userName(""), _nickName(""), _realName(""), _isOperator(0)
{
	
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
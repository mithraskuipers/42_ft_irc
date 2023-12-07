#include "Channel.hpp"

void Channel::printChannelPrivates() // REMOVE LATER
{
	std::cout << std::endl << "Channel: printing: channelName, isInviteOnly" << std::endl;
	std::cout << _channelName << std::endl;
	std::cout << _isInviteOnly << std::endl << std::endl;
}

Channel::Channel(std::string channelName) : _channelName(channelName), _isInviteOnly(0)
{

}

 std::string Channel::getChannelName()
 {
	return (_channelName);
 }

 bool Channel::getIsInviteOnly()
 {
	return (_isInviteOnly);
 }

// bool Channel::findUser(int userFD)
// {
//	loop through _joinedUserFDs, compare to argument
// 	return 1 if found
// 	return 0 if not found
// }

// bool Channel::findBannedUser(int bannedUserFD)
// {
//	loop through _bannedUserFDs, compare to argument
// 	return 1 if found
// 	return 0 if not found
// }

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
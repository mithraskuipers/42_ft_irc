#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>

class Channel
{
	public:
	Channel(std::string channelName);
	~Channel();

	void msgAllInChannel(std::string msg);

	void addToChannel(int userFD);
	void rmvFromChannel(int userFD);

	void addToOperators(int userFD);
	void rmvFromOperators(int userFD);

	void addToBanned(int userFD);
	void rmvFromBanned(int userFD);

	bool isInChannel(int userFD);
	bool isOperator(int userFD);
	bool isBanned(int userFD);

	std::string	getTopic();
	std::string	getChannelName();
	std::string	getChannelKey();
	std::string	getActiveModes();
	bool		getIsInviteOnly();
	int			getLimit();
	int			getNumOfUsers();
	int			getNumOfOperators();
	int			getFirstJoinedUserFD();

	void setLimit(int limit);
	void setTopic(std::string topic);
	void setChannelName(std::string channelName);
	void setChannelKey(std::string channelKey);
	void setActiveModes(std::string modes);
	void setIsInviteOnly(bool inviteBool);

	private:
	std::string		_topic;
	std::string		_channelName;
	std::string		_channelKey;
	std::string		_activeModes;
	int				_intArr[4];
	int				_limit;

	std::vector<int> _joinedUserFDs;
	std::vector<int> _operatorFDs;
	std::vector<int> _bannedUserFDs;
};

#endif
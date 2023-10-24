#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class User;

class Channel
{
public:
	Channel();
	~Channel();
	Channel(const std::string &name);
	void addUser(User *user);
	void removeUser(User *user);
	bool isEmpty() const;
	void listUsers() const;
	std::string getName() const;
	void setTopic(const std::string &topic, User *user);
	std::string getTopic() const;
	std::string getTopicAuthor() const;
	bool isOperator(User *user) const;
	void addOperator(User *user);
	void removeOperator(User *user);
	int getUsersCount() const;
	bool isUserInChannel(const User *user) const;
	const std::vector<User *> &getUsers() const { return users; }
	void broadcastMessage(const std::string &message, User *sender);

private:
	std::vector<User *> users;
	std::string channelName;
	std::string topic;
	std::string topicAuthor;
	std::vector<User *> operators;
};

#endif // CHANNEL_HPP

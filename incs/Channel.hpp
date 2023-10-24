#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <algorithm> // Include the necessary header for std::remove

class Client;

class Channel
{
	public:
		Channel();
		~Channel();
		Channel(const std::string &name);
		void addUser(Client *user);
		void removeUser(Client *user);
		bool isEmpty() const;
		void listUsers() const;
		std::string getName() const;
		void setTopic(const std::string &topic, Client *user);
		std::string getTopic() const;
		std::string getTopicAuthor() const;
		bool isOperator(Client *user) const;
		void addOperator(Client *user);
		void removeOperator(Client *user);
		int getUsersCount() const;
		bool isUserInChannel(const Client *user) const;
		const std::vector<Client *> &getUsers() const { return users; }
		void broadcastMessage(const std::string &message, Client *sender);

	private:
		std::vector<Client *> users;
		std::string channelName;
		std::string topic;
		std::string topicAuthor;
		std::vector<Client *> operators;
};

#endif
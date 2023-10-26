#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <algorithm> // std::remove

class Client;

class Channel
{
	public:
		Channel(const std::string &name);
		~Channel();
		Channel();

		bool isClientInChannel(const Client *client) const;
		bool isOperator(Client *client) const;
		void removeOperator(Client *client);
		void addOperator(Client *client);
		void removeClient(Client *client);
		void addClient(Client *client);
		void listClients() const;
		bool isEmpty() const;

		void broadcastMessage(const std::string &message, Client *sender);
		void setTopic(const std::string &topic, Client *client);

		const std::vector<Client *> &getClients() const { return _clients; }
		std::string getTopicAuthor() const;
		std::string getTopic() const;
		std::string getName() const;
		int getClientsCount() const;

	private:
		std::vector<Client *> operators;
		std::vector<Client *> _clients;
		std::string channelName;
		std::string topicAuthor;
		std::string topic;
};

#endif
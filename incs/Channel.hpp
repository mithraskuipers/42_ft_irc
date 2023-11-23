/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:38 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:39 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

class Channel;

#include "Server.hpp"
#include "Client.hpp"
#include <list>

class Channel
{
	typedef std::vector<Client *>::iterator clients_iterator;

private:
	std::string channelName;
	Client *channelAdminClient;
	std::vector<Client *> _clients;

	std::string channelPassword;
	size_t channelMaxClients;
	bool channelAllowExternalMessages;
	bool channelInviteOnly;
	std::list<Client *> channelInvs;

public:
	Channel(const std::string &name, const std::string &password, Client *admin);
	~Channel();

	Client* getAdmin();
	std::string getChannelName() const;
	std::string getPassword() const;
	void setPassword(std::string password);
	void channelSetInviteOnly(bool inviteOnly);
	bool channelGetInviteOnly() const;
	void addInvite(Client *client);
	void removeInvite(Client *client);
	bool checkInvited(Client *client);
	size_t getMaxClients() const;
	void setMaxClients(size_t nmaxclients);
	bool allowExternalMessagesToChannel() const;
	size_t getNumClients() const;
	std::vector<std::string> getNicknames();

	void singleBroadcast(const std::string &firstMessageCombined, std::string include);
	// removeClient()
	void selectNewAdmin();
    void logNewAdmin();
    void logEmptyChannel();

	// ONLY KEEP THE FIRST ONE, MAYBE THE EXCLUDE ONE IS NOT NECESSARY
    void channelBroadcast(const std::string &firstMessageCombined, Client* exclude = nullptr);

	void removeClient(Client *client);
	void addClient(Client *client);
};

#endif

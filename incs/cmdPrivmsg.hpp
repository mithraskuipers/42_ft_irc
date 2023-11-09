#ifndef CMDPRIVMSG_HPP
# define CMDPRIVMSG_HPP

#include "./../incs/Command.hpp"

class cmdPrivmsg : public Command
{
	public:
		cmdPrivmsg(Server *server, bool registrationRequired);
		~cmdPrivmsg();

		void run(Client *client, std::vector<std::string> arguments);
		std::string argumentPlakker(const std::vector<std::string> &arguments);
		void sendMessageToChannel(Client *client, const std::string &target, const std::string &message);
		void sendMessageToPrivateUser(Client *client, const std::string &target, const std::string &message);
};

# endif
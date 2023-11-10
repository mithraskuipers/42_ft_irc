#ifndef CMDUSER_HPP
# define CMDUSER_HPP

class cmdUser : public Command
{
	public:
		cmdUser(Server *server, bool registrationRequired);
		~cmdUser();

		void execute(Client *client, std::vector<std::string> arguments);
};

#endif
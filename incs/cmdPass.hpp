#ifndef CMDPASS_HPP
#define CMDPASS_HPP

class cmdPass : public Command
{
	public:
		cmdPass(Server *server, bool registrationRequired);
		~cmdPass();

		void run(Client *client, std::vector<std::string> arguments);
};

#endif
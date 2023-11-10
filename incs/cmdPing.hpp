#ifndef CMDPING_HPP
#define CMDPING_HPP

class cmdPing : public Command
{
	public:
		cmdPing(Server *server, bool registrationRequired);
		~cmdPing();

		void execute(Client *client, std::vector<std::string> arguments);
};


#endif
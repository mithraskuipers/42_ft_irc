#ifndef CMDNICK_HPP
# define CMDNICK_HPP

class cmdNick : public Command
{
public:
	cmdNick(Server *server, bool registrationRequired);
	~cmdNick();

	void run(Client *client, std::vector<std::string> arguments);
};


#endif 
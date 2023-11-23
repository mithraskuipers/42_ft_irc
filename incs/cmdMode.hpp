#ifndef CMDMODE_HPP
# define CMDMODE_HPP

class cmdMode : public Command
{
public:
	cmdMode(Server *server, bool registrationRequired);
	~cmdMode();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif
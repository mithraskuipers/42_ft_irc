#ifndef CMDKICK_HPP
# define CMDKICK_HPP

class cmdKick : public Command
{
public:
	cmdKick(Server *server, bool registrationRequired);
	~cmdKick();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif
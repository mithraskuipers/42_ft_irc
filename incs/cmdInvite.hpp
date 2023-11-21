#ifndef CMDINVITE_HPP
# define CMDINVITE_HPP

class cmdInvite : public Command
{
public:
	cmdInvite(Server *server, bool registrationRequired);
	~cmdInvite();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif
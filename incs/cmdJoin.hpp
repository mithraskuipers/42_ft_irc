#ifndef CMDJOIN_HPP
# define CMDJOIN_HPP

class cmdJoin : public Command
{
public:
	cmdJoin(Server *server, bool registrationRequired);
	~cmdJoin();

	void run(Client *client, std::vector<std::string> arguments);
};

#endif
#ifndef CMDJOIN_HPP
# define CMDJOINexecute_HPP

class cmdJoin : public Command
{
public:
	cmdJoin(Server *server);
	~cmdJoin();

	void run(Client *client, std::vector<std::string> arguments);
};

#endif
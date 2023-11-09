#ifndef CMDPART_HPP
# define CMDPART_HPP

class cmdPart : public Command
{
public:
	cmdPart(Server *server, bool registrationRequired);
	~cmdPart();

	void run(Client *client, std::vector<std::string> arguments);
};

#endif
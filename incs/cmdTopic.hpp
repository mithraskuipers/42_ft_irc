#ifndef CMDTOPIC_HPP
# define CMDTOPIC_HPP

class cmdTopic : public Command
{
public:
	cmdTopic(Server *server, bool registrationRequired);
	~cmdTopic();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif
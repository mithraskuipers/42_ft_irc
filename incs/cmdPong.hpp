#ifndef CMDPONG_HPP
#define CMDPONG_HPP

class cmdPong : public Command
{
	public:
		cmdPong(Server *server, bool registrationRequired);
		~cmdPong();

		void execute(Client *client, std::vector<std::string> arguments);
};

#endif
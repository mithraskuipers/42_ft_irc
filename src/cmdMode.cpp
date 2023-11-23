#include "./../incs/Command.hpp"
#include "./../incs/cmdMode.hpp"
#include "./../incs/Replies.hpp"

// command KICK

cmdMode::cmdMode(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
	
}

cmdMode::~cmdMode()
{
	
}
// source, channel, modes, args
void cmdMode::execute(Client *client, std::vector<std::string> arguments)
{
    if (arguments.size() > 1)
    {
        if (arguments[0] == "i")
        {
            client->setMode("i", arguments[1], "");
        }
    }
}
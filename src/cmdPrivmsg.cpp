#include "./../incs/Command.hpp"
#include "./../incs/cmdPrivmsg.hpp"
#include "./../incs/Replies.hpp"

cmdPrivmsg::cmdPrivmsg(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{

}

cmdPrivmsg::~cmdPrivmsg()
{

}

/*
Voorbeeld van arguments string vector is bijv:
"#channel", "Hello", "this", "is", "a", "channel", "firstMessageCombined!"
De persoon die het verstuurde had dan het volgende gestuurd:
"/privmsg #channel Hello this is a channel firstMessageCombined!"
*/

void cmdPrivmsg::execute(Client *client, std::vector<std::string> arguments)
{
	std::string target;															// Var to store the firstMessageCombined target (channel or user)
	std::string firstMessageCombined;														// Variable to store the actual firstMessageCombined itself

	if ((arguments.size() < 2) || (arguments[0].empty()) || (arguments[1].empty()))
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PRIVMSG")); // Not enough args, send error to client and return
		return ;
	}
	target = arguments.at(0);													// Extract the target (i.e. first string) from arguments
	firstMessageCombined = argumentPlakker(arguments);										// Concatenate the firstMessageCombined content using helper function

	if (target.at(0) == '#')
	{
		sendMessageToChannel(client, target, firstMessageCombined);							// Handle channel firstMessageCombined
	}
	else
	{
		sendMessageToPrivateUser(client, target, firstMessageCombined);						// Handle private firstMessageCombined
	}
}

// [client] stuurt /privmsg henk lekker bezig -> [irssi] vertaalt dat naar PRIVMSG #henk :lekker bezig -> [ontvangende client] ontvangt ":lekker bezig", en niet "lekker bezig". Handmatig ":" verwijderen dus.

std::string cmdPrivmsg::argumentPlakker(const std::vector<std::string> &privmsgArgs)
{
	std::string firstMessageCombined;
	for (auto it = privmsgArgs.begin() + 1; it != privmsgArgs.end(); ++it)
	{
		firstMessageCombined.append(*it + " ");
	}
	if (!firstMessageCombined.empty() && firstMessageCombined.at(0) == ':')								// Raar iets van IRC protocol. Als je priv messages stuurt dan plakt IRC client een ":" voor het bericht.
	{
		firstMessageCombined = firstMessageCombined.substr(1);											// Remove the leading colon from the firstMessageCombined
	}
	return (firstMessageCombined);
}

void cmdPrivmsg::sendMessageToChannel(Client *client, const std::string &targetChannelName, const std::string &firstMessageCombined)
{
	Channel *targetedChannel = client->getChannelInstance();
	if (!targetedChannel)
	{
		client->sendMessageToClientItself(ERR_NOSUCHCHANNEL(client->getNickname(), targetChannelName));
		return;
	}

	if (!client->isInChannel(targetChannelName) && !targetedChannel->allowExternalMessagesToChannel())
	{
		client->sendMessageToClientItself(ERR_CANNOTSENDTOCHAN(client->getNickname(), targetChannelName));
		return;
	}

	targetedChannel->channelBroadcast(RPL_PRIVMSG(client->getPrefix(), targetChannelName, firstMessageCombined), client);
}

/*
Als client A een bericht wil sturen naar client B, dan gebruikt client A de send() met client B's socket als argument
*/

void cmdPrivmsg::sendMessageToPrivateUser(Client *client, const std::string &targetClientNickname, const std::string &firstMessageCombined)
{
	Client *targetedClientObject = server->getClientInstance(targetClientNickname);
	if (!targetedClientObject)
	{
		client->sendMessageToClientItself(ERR_NOSUCHNICK(client->getNickname(), targetClientNickname));
		return ;
	}

	// Trucje om een bericht te sturen naar een andere client. Roep gewoon binnen die client zijn eigen sendMessageToClientItself() functie aan met als argument het bericht.
	targetedClientObject->sendMessageToClientItself(RPL_PRIVMSG(client->getPrefix(), targetClientNickname, firstMessageCombined));
}
#include "./../incs/IRCServer.hpp" // Include your IRCServer.hpp file after the necessary system
#include "IRCServer.hpp"

#include "Command.hpp"

class IRCServer;

IRCServer::IRCServer(int port, const std::string &password)
	: active_users(0),
	  port(port),
	  clients(),
	  password(password),
	  command(clients, *this),
	  server_listening_socket(0),
	  IP(),
	  socket_address(),
	  channels(),
	  welcomeMessage("Welcome to the IRC server!")
{
	// Rest of your constructor implementation
}

std::string IRCServer::getPass()
{
	return (this->password);
}

IRCServer::~IRCServer()
{
}

void IRCServer::getHostIP()
{
	char host[256];
	gethostname(host, sizeof(host));

	struct hostent *host_entry;
	host_entry = gethostbyname(host);

	this->IP = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
}

void IRCServer::initServer()
{
	int opt = 1;
	this->server_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_listening_socket == 0)
	{
		throw(std::runtime_error("Failed to create socket"));
	}
	int setsockopt_ret = setsockopt(server_listening_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (setsockopt_ret == -1)
	{
		throw(std::runtime_error("Failed to set socket options"));
	}
	this->socket_address.sin_family = AF_INET;
	this->socket_address.sin_addr.s_addr = INADDR_ANY;
	this->socket_address.sin_port = htons(port);

	if (fcntl(server_listening_socket, F_SETFL, O_NONBLOCK) < -1)
	{
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	if (bind(server_listening_socket, (struct sockaddr *)&this->socket_address, sizeof(this->socket_address)) < 0)
	{
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(server_listening_socket, QUEUE_SIZE) == -1)
	{
		throw std::runtime_error("Failed to start listening");
	}

	std::cout << "Listening on port: " << this->port << std::endl;
	std::cout << "The password you chose to use is: " << this->password << std::endl;
}

/*
generateRandomCode() for creating a random code for the "Guest" username.
*/
std::string IRCServer::generateRandomCode()
{
	std::string randomCode;
	srand(time(0)); // Seed for random number generation based on current time

	// Generate a random code with a length of 4 digits
	for (int i = 0; i < 4; ++i)
	{
		char randomDigit = '0' + rand() % 10; // Generate a random digit
		randomCode.push_back(randomDigit);
	}

	return randomCode;
}

/*
isNicknameInUse() checks if a username has already been taken.
*/
bool IRCServer::isNicknameInUse(const std::string &nickname) const
{
	for (const auto &User : clients)
	{
		if (User.getNick() == nickname)
		{
			return true;
		}
	}
	return false;
}

/*
updateMaxSocketDescriptor() Finds maximum socket descriptor value among all connected clients.
Essential for select(), which is used to monitor multiple file descriptors for read readiness.
*/
int IRCServer::updateMaxSocketDescriptor()
{
	int maxSocket = server_listening_socket;

	for (const auto &User : clients)
	{
		if (User.getSocketDescriptor() > maxSocket)
		{
			maxSocket = User.getSocketDescriptor();
		}
	}
	return maxSocket;
}

std::string IRCServer::addClientSocket(int clientSocket)
{
	std::string welcomeMessage;
	const std::string defaultNickname = "Guest";
	std::string randomCode = generateRandomCode();
	std::string username = defaultNickname + randomCode;

	welcomeMessage = "IRC : To register please use commands PASS - NICK - USER(user, mode, unused, realname)\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	while (isNicknameInUse(username))
	{
		randomCode = generateRandomCode();
		username = defaultNickname + randomCode;
	}

	clients.push_back(User(clientSocket, username));

	welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + username + " :Welcome to the IRC server, " + username + "!\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	return (username);
}

void IRCServer::startServer()
{
	std::vector<char> buffer(2048);

	while (true)
	{
		int readySockets;
		int max_socket_fd;
		int client_socket;
		std::string username;
		std::string welcomeMessage;
		std::string motdMessage;
		std::string motdContent;
		std::string endMotdMessage;
		int bytes_received;
		fd_set fd_pack;
		FD_ZERO(&fd_pack);
		FD_SET(server_listening_socket, &fd_pack);

		max_socket_fd = updateMaxSocketDescriptor();

		for (const auto &User : clients)
		{
			FD_SET(User.getSocketDescriptor(), &fd_pack);
		}

		readySockets = select(max_socket_fd + 1, &fd_pack, NULL, NULL, NULL);

		if (readySockets == -1)
		{
			std::cerr << "Error: Failed to select sockets: " << strerror(errno) << std::endl;
			break;
		}

		if (FD_ISSET(server_listening_socket, &fd_pack))
		{
			client_socket = accept(server_listening_socket, NULL, NULL);

			if (client_socket == -1)
			{
				std::cerr << "Error: Failed to accept client connection: " << strerror(errno) << std::endl;
				continue;
			}
			else
			{
				username = addClientSocket(client_socket);
				std::cout << "New User connected. Total clients: " << clients.size() << std::endl;

				// Send IRC welcome messages to the client
				welcomeMessage = ":" + std::string(SERVER_NAME) + " 001 " + username + " :Welcome to the IRC server, " + username + "!\r\n";
				send(client_socket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

				// Send Message of the Day message to client
				motdMessage = ":" + std::string(SERVER_NAME) + " 375 " + username + " :- " + std::string(SERVER_NAME) + " Message of the Day -\r\n";
				send(client_socket, motdMessage.c_str(), motdMessage.size(), 0);
				motdContent = ":" + std::string(SERVER_NAME) + " 372 " + username + " :- Welcome to our awesome IRC server! yugioh > magic!!! Enjoy your stay.\r\n";
				send(client_socket, motdContent.c_str(), motdContent.size(), 0);
				endMotdMessage = ":" + std::string(SERVER_NAME) + " 376 " + username + " :End of /MOTD command.\r\n";
				send(client_socket, endMotdMessage.c_str(), endMotdMessage.size(), 0);
			}
		}

		// For each connected client, check if everything is set up and if they sent data.
		for (auto it = clients.begin(); it != clients.end();)
		{
			client_socket = it->getSocketDescriptor();

			if (FD_ISSET(client_socket, &fd_pack))
			{
				bytes_received = recv(client_socket, buffer.data(), buffer.size(), 0);

				if (bytes_received <= 0)
				{
					if (bytes_received == 0)
					{
						std::cout << "Client disconnected. Total clients: " << clients.size() - 1 << std::endl;
					}
					else
					{
						std::cerr << "Error: Failed to receive data from client: " << strerror(errno) << std::endl;
					}
					close(client_socket);
					it = clients.erase(it); // Remove the client from the list
					continue;
				}

				// Append received data to the client's buffer
				it->getBuff().append(buffer.data(), bytes_received);

				// Process complete messages in the buffer
				size_t newlinePos;
				while ((newlinePos = it->getBuff().find('\n')) != std::string::npos)
				{
					std::string complete_command = it->getBuff().substr(0, newlinePos);

					std::cout << "Command received, socket fd : " << client_socket << ", IP : " << it->getIP() << ", port : " << it->getPort() << std::endl;
					command.commandHandler(complete_command, *it);

					// Remove processed message from the buffer
					it->getBuff().erase(0, newlinePos + 1);
				}

				// Check if the buffer size exceeds the allowed limit
				if (it->getBuff().size() > BUFF_LIMIT)
				{
					std::string errorMessage = "ERROR : Message size exceeds the allowed limit.\r\n";
					send(client_socket, errorMessage.c_str(), errorMessage.size(), 0);

					// Clear the buffer to avoid further processing of this message
					it->getBuff().clear();
				}
			}
			it++;
		}
	}
}
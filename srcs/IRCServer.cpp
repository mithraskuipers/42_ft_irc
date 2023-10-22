#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>		 // Add this line to include the netdb.h header
#include "./../incs/IRCServer.hpp" // Include your IRCServer.hpp file after the necessary system

IRCServer::IRCServer(int port, const std::string &password) : port(port), password(password), clients(), command(clients, *this)
{
    // Initialize other members as needed
}

std::string IRCServer::getPass()
{
	return (this->password);
}

IRCServer::~IRCServer()
{
	// Destructor logic if needed
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
		throw std::runtime_error("Failed to create socket");
	}

	int setsockopt_ret = setsockopt(server_listening_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (setsockopt_ret == -1)
	{
		throw std::runtime_error("Failed to set socket options");
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

#include <cstdlib>
#include <ctime>

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

std::string IRCServer::addClientSocket(int clientSocket) // Update the definition
{
	std::string defaultNickname = "Guest";
	std::string randomCode;
	std::string welcomeMessage = "IRC : To register please use commands PASS - NICK - USER(user, mode, unused, realname)";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	// Generate a random number code and check if it's not already in use
	do
	{
		randomCode = generateRandomCode();
	} while (isNicknameInUse(defaultNickname + randomCode));

	defaultNickname = defaultNickname + randomCode;
	clients.push_back(User(clientSocket, defaultNickname));

	return defaultNickname; // Return the generated username
}


void IRCServer::start() {
    std::vector<char> buffer(2048);

    while (true) {
        fd_set fd_pack;
        FD_ZERO(&fd_pack);
        FD_SET(server_listening_socket, &fd_pack);

        int max_socket_fd = updateMaxSocketDescriptor();

        for (const auto &User : clients) {
            FD_SET(User.getSocketDescriptor(), &fd_pack);
        }

        int readySockets = select(max_socket_fd + 1, &fd_pack, NULL, NULL, NULL);

        if (readySockets == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(server_listening_socket, &fd_pack)) {
            int socket_descriptor = accept(server_listening_socket, NULL, NULL);

            if (socket_descriptor == -1) {
                perror("accept");
                continue;
            } else {
                std::string username = addClientSocket(socket_descriptor);
                std::cout << "New User connected. Total clients: " << clients.size() << std::endl;

                // Send IRC welcome messages to the client
                std::string welcomeMessage = ":randomservernaam 001 " + username + " :Welcome to the IRC server, " + username + "!\r\n";
                send(socket_descriptor, welcomeMessage.c_str(), welcomeMessage.size(), 0);

                // Send MOTD (Message of the Day) message to the client
                std::string motdMessage = ":randomservernaam 375 " + username + " :- randomservernaam Message of the Day -\r\n";
                send(socket_descriptor, motdMessage.c_str(), motdMessage.size(), 0);

                // Send MOTD content to the client (customize this according to your server's MOTD)
                std::string motdContent = ":randomservernaam 372 " + username + " :- Welcome to our IRC server! Enjoy your stay.\r\n";
                send(socket_descriptor, motdContent.c_str(), motdContent.size(), 0);

                // Send end of MOTD message
                std::string endMotdMessage = ":randomservernaam 376 " + username + " :End of /MOTD command.\r\n";
                send(socket_descriptor, endMotdMessage.c_str(), endMotdMessage.size(), 0);
            }
        }

        // Inside your while loop where you handle incoming data and disconnections
        for (auto it = clients.begin(); it != clients.end();) {
            int socket_descriptor = it->getSocketDescriptor();

            if (FD_ISSET(socket_descriptor, &fd_pack)) {
                int bytes_received = recv(socket_descriptor, buffer.data(), buffer.size(), 0);

                if (bytes_received <= 0) {
                    if (bytes_received == 0) {
                        std::cout << "Client disconnected. Total clients: " << clients.size() - 1 << std::endl;
                    } else {
                        perror("recv");
                    }
                    close(socket_descriptor);
                    it = clients.erase(it); // Remove the client from the list
                    continue;
                }

                // Append received data to the client's buffer
                it->getBuff().append(buffer.data(), bytes_received);

                // Process complete messages in the buffer
                size_t newlinePos;
                while ((newlinePos = it->getBuff().find('\n')) != std::string::npos) {
                    std::string complete_command = it->getBuff().substr(0, newlinePos);

                    // Process the complete command
                    std::cout << "Command received, socket fd : " << socket_descriptor << ", IP : " << it->getIP() << ", port : " << it->getPort() << std::endl;
                    command.process(complete_command, *it, channels);

                    // Remove processed message from the buffer
                    it->getBuff().erase(0, newlinePos + 1);
                }

                // Check if the buffer size exceeds the allowed limit
                if (it->getBuff().size() > 1024) {
                    std::string errorMessage = "ERROR : Message size exceeds the allowed limit.\r\n";
                    send(socket_descriptor, errorMessage.c_str(), errorMessage.size(), 0);

                    // Clear the buffer to avoid further processing of this message
                    it->getBuff().clear();
                }
            }

            it++;
        }
    }
}

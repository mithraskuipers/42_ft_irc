/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/08/29 10:42:04 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/10/17 22:49:13 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h> // gethostname()
#include <netdb.h> // gethostbyname()
#include <arpa/inet.h> // inet_ntoa()
#include <netinet/in.h> // struct sockaddr_in
#include <fcntl.h>  // file control options
#include <sys/types.h> // addrlen
#include "./../incs/includes.hpp"
#include <cstring>
#include <ctime> // Include the <ctime> header for time-related functions


IRCServer::IRCServer() : port(1234), password("")
{
	this->getHostIP();
}

IRCServer::IRCServer(int port, const std::string &password)
{
	this->port = port;
	this->password = password;
	this->getHostIP();
}

IRCServer::~IRCServer()
{
	// 
}

/*
Retrieves the primary IP socket_address of the current machine and stores it in the 'IP' member variable.
*/

void IRCServer::getHostIP()
{
	// Buffer for hostname
	char host[256]; // TODO: MAKE THIS DYNAMIC ALLOCATION?
	
	// Step 1: Get the hostname of the current machine and save it in the 'host' buffer
	gethostname(host, sizeof(host));
	
	// Step 2: Retrieve host information
	// Step 2.1 Pointer to hostent struct that stores host information
	struct hostent *host_entry;
	
	// Step 2.2: Retrieve host information using the hostname
	host_entry = gethostbyname(host);
	
	// Convert binary IP socket_address to string
	// Note: h_addr_list[0] contains the primary IP socket_address associated with the hostname
	this->IP = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));

	// [DEBUGGING]: Print host ip
	std::cout << "Host IP is: " << this->IP << std::endl;
}

/*
This function sets up the socket, configures options, and prepares for incoming connections.
*/

#include <stdio.h>

void IRCServer::initServer()
{
	// Step 1: Set up socket and options
	int opt = 1;  // Option for setsockopt
	this->server_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_listening_socket == 0) // Use IPv4 socket family and TCP type socket
	{
		throw std::runtime_error("Failed to create socket");
	}
		
	// Step 2: Set socket options to allow reusing the socket_address
	int setsockopt_ret = setsockopt(server_listening_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	if (setsockopt_ret == -1)
	{
		throw std::runtime_error("Failed to set socket options");
	}

	// Step 3: Set up socket_address structure
	this->socket_address.sin_family = AF_INET;                // Use IPv4 socket family
	this->socket_address.sin_addr.s_addr = INADDR_ANY;        // Listen on any available network interface
	this->socket_address.sin_port = htons(port);              // Set the port number in network byte order

	// Step 4: Set socket to non-blocking mode
	// Uses file control function to set properties of a socket
	if (fcntl(server_listening_socket, F_SETFL, O_NONBLOCK) < -1)
	{
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	// Step 5: Bind the socket to the socket_address
	// Step 5.1: Convert the socket_address structure to a pointer of type struct sockaddr *
	struct sockaddr *socket_address = (struct sockaddr *)&this->socket_address;
	
	// Step 5.2: Get the size of the socket_address structure
	socklen_t address_size = sizeof(this->socket_address);
	
	// Step 5.3 Attempt to bind the socket to the socket_address
	if (bind(this->server_listening_socket, socket_address, address_size) < 0)
	{
		throw std::runtime_error("Failed to bind socket");
	}

	// Print listening information
	std::cout << "Listening on port: " << this->port << std::endl;
	std::cout << "The password you chose to use is: " << this->password << std::endl;

	// Step 6: Start listening for incoming connections
	if (listen(this->server_listening_socket, QUEUE_SIZE) == -1)
	{
		throw std::runtime_error("Failed to start listening");
	}
	std::cout << "Waiting for connection.." << std::endl << std::endl;
}

// This function accepts incoming connections.


std::string IRCServer::getIP() const
{
	return (this->IP);
}

int IRCServer::getPort() const
{
	return (this->port);
}

void print_waiting_status(const IRCServer &other)
{
	time_t raw_datetime = std::time(NULL);
	struct tm datetime;
	localtime_r(&raw_datetime, &datetime);

	char time_buffer[9];
	strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &datetime);

	// [DEBUGGING]: Show that the server is waiting and also show how to connect
	std::cout << "\nWaiting...[" << time_buffer << "]" << std::endl;
	std::cout << "Server IP is: [" << other.getIP() << "]" << std::endl;
	std::cout << "Server port is: [" << other.getPort() << "]" << std::endl;

	sleep(1);
}

void IRCServer::addClientSocket(int clientSocket) {
	if (nConnectedClients < MAX_CLIENTS) {
		clients.push_back(Client(clientSocket));
		nConnectedClients++;
	} else {
		// TODO? Triggers when max clients has been reached...
	}
}

int IRCServer::updateMaxSocketDescriptor()
{
	int maxSocket = server_listening_socket; // Initialize with the server's listening socket

	// Iterate through all connected clients using the std::list
	for (const auto& client : clients)
	{
		if (client.getSocketDescriptor() > maxSocket)
		{
			maxSocket = client.getSocketDescriptor();
		}
	}

	return maxSocket; // Return the maximum socket descriptor value
}

/*
Server Socket:
This is the socket used by the server program to listen for incoming connections from clients.
It's also known as the listening socket. The server socket is passive and waits for clients to connect to it.

Client Socket:
This is the socket used by the client program to initiate a connection to the server.
It's an active socket that initiates the communication by connecting to the server's listening socket.
*/


#include <vector>

void IRCServer::start()
{
	fd_set fd_pack;
    std::vector<char> buffer(1024); // Start with an initial size of 1024 bytes
	int max_socket_fd;
	int readySockets;

	while (true)
	{
		// Clear the fd_set and add the server's listening socket
		FD_ZERO(&fd_pack);
		FD_SET(server_listening_socket, &fd_pack);

		// Call updateMaxSocketDescriptor to get the maximum socket descriptor value
		max_socket_fd = updateMaxSocketDescriptor();

		// Add connected client sockets to the fd_set
		for (const auto& client : clients)
		{
			FD_SET(client.getSocketDescriptor(), &fd_pack);
		}

		// Monitor fd's
		readySockets = select(max_socket_fd + 1, &fd_pack, NULL, NULL, NULL);

		// Handle select() errors
		if (readySockets == -1)
		{
			perror("select");
			break;
		}

		// Check if server's listening socket is in fd_set (new client attempting to connect)
		if (FD_ISSET(server_listening_socket, &fd_pack))
		{
			// Accept new client connection
			int socket_descriptor = accept(server_listening_socket, NULL, NULL);
			if (socket_descriptor == -1)
			{
				perror("accept");
			}
			else
			{
				// Add the new client to the clients list
				clients.push_back(Client(socket_descriptor));
				std::cout << "New client connected. Total clients: " << clients.size() << std::endl;
			}
		}

        // Check for activity on client sockets and handle data
        for (auto it = clients.begin(); it != clients.end();)
        {
            int socket_descriptor = it->getSocketDescriptor();
            if (FD_ISSET(socket_descriptor, &fd_pack))
            {
                // Receive data into the dynamic buffer
                int bytes_received = recv(socket_descriptor, buffer.data(), buffer.size(), 0);
                if (bytes_received <= 0)
                {
                    // Client disconnected or error occurred, remove client from the list
                    close(socket_descriptor);
                    it = clients.erase(it);
                    std::cout << "Client disconnected. Total clients: " << clients.size() << std::endl;
                    continue;
                }
                else
                {
                    // Handle received data here (buffer contains the received data)
                    // Process received data as needed

                    buffer[bytes_received] = '\0'; // Null-terminate the received data

                    // Process the received data using buffer.data()

                    // Check if the buffer is almost full, and if so, double its size
                    if (bytes_received >= buffer.size() - 1)
                    {
                        buffer.resize(buffer.size() * 2);
                    }
                }
            }
            ++it;
        }
    }
}







/*
irssi
/connect 127.0.1.1 1234
*/
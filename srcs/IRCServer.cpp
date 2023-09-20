/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/08/29 10:42:04 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/09/20 13:55:29 by mikuiper      ########   odam.nl         */
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

void IRCServer::addClientSocket(int clientSocket)
{
	if (nConnectedClients < MAX_CLIENTS)
	{
		client_socket_array[nConnectedClients++] = clientSocket;
	}
	else
	{
		// TODO? Triggers when max clients has been reached...
	}
}

int IRCServer::updateMaxSocketDescriptor()
{
	int maxSocket = server_listening_socket; // Initialize with the server's listening socket

	// Iterate through all connected clients
	for (const auto& clientSocket : client_socket_array)
	{
		if (clientSocket > maxSocket)
		{
			maxSocket = clientSocket;
		}
	}

	return (maxSocket); // Return the maximum socket descriptor value
}

/*
Server Socket:
This is the socket used by the server program to listen for incoming connections from clients.
It's also known as the listening socket. The server socket is passive and waits for clients to connect to it.

Client Socket:
This is the socket used by the client program to initiate a connection to the server.
It's an active socket that initiates the communication by connecting to the server's listening socket.
*/

void IRCServer::start()
{
	fd_set fd_pack;

	char buffer[9999]; // TODO? HOW MAKE DYNAMICALLY SIZED BUFFER? BUFFER EVEN NECESSARY?
	int socket_descriptor;
	int max_socket_fd;
	int readySockets; // n sockets that are ready for I/O operations in fd_pack.
	while (true)
	{
		// Clear the fd_set and add the server's listening socket
		FD_ZERO(&fd_pack);
		FD_SET(this->server_listening_socket, &fd_pack);

		// Call updateMaxSocketDescriptor to get the maximum socket descriptor value
		max_socket_fd = updateMaxSocketDescriptor();

		// Add connected client sockets to the fd_set
		for (int i = 0; i < nConnectedClients; i++)
		{
			FD_SET(client_socket_array[i], &fd_pack);
		}
		// Monitor fd's
		readySockets = select(max_socket_fd + 1, &fd_pack, NULL, NULL, NULL);
		// Check if select() failed
		if (readySockets == -1)
		{
			// Handle the error here, e.g., print an error message or exit the server
			perror("select");
			break;
		}

		// Check if FD_SET succeeded using FD_ISSET(); Only run accept() when FD_ISSET().
		if (FD_ISSET(this->server_listening_socket, &fd_pack))
		{
			// A new client is attempting to connect
			socket_descriptor = accept(this->server_listening_socket, NULL, NULL);
			if (socket_descriptor == -1)
			{
				// Handle the error here, e.g., print an error message
				perror("accept");
			}
			else
			{
				// A new client has successfully connected
				std::cout << "Iemand is verbonden.." << std::endl;
				this->addClientSocket(socket_descriptor);
				std::cout << this->nConnectedClients << std::endl; // Print the number of connected clients
			}
		}
		

		// Check for activity on client sockets and handle data as before
		for (int i = 0; i < nConnectedClients; i++)
		{
			socket_descriptor = client_socket_array[i];
			if (FD_ISSET(socket_descriptor, &fd_pack))
			{
				int bytes_received = recv(socket_descriptor, buffer, sizeof(buffer), 0);
				if (bytes_received == -1)
				{
					std::cout << "An error occurred" << std::endl;
				}
				else
				{
					// Print the received data directly
					std::cout << "Received data: " << buffer << std::endl;

					// Check if the received data contains the escape character (e.g., 'ESC' key)
					if (bytes_received >= 1 && buffer[0] == 27) // 27 is ASCII for ESC
					{
						// Send a "Disconnect" message to the client
						const char* disconnect_message = "Disconnect";
						send(socket_descriptor, disconnect_message, strlen(disconnect_message), 0);

						// Close the connection and clean up the socket
						close(socket_descriptor);
						std::cout << "Client disconnected using ESC key." << std::endl;
						continue; // Continue to accept other connections
					}
				}
			}
		}
	}
}









/*
irssi
/connect 127.0.1.1 1234
*/
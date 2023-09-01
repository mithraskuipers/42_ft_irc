/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/08/29 10:42:04 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/09/01 13:22:01 by mkuipers      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"
#include "./../incs/IRCServer.hpp"

#include <unistd.h> // gethostname()
#include <netdb.h> // gethostbyname()
#include <arpa/inet.h> // inet_ntoa()
#include <netinet/in.h> // struct sockaddr_in
#include <fcntl.h>  // file control options
#include <sys/types.h> // addrlen


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

/*
Retrieves the primary IP socket_address of the current machine and stores it in the 'IP' member variable.
*/

void IRCServer::getHostIP()
{
    // Buffer for hostname
    char host[256];
    
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
}

/*
This function sets up the socket, configures options, and prepares for incoming connections.
*/

#include <stdio.h>

void IRCServer::initServer()
{
    // Step 1: Set up socket and options
    int opt = 1;  // Option for setsockopt
    this->main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->main_socket == 0) // Use IPv4 socket family and TCP type socket
    {
        throw std::runtime_error("Failed to create socket");
    }
        
    // Step 2: Set socket options to allow reusing the socket_address
    if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        throw std::runtime_error("Failed to set socket options");

    // Step 3: Set up socket_address structure
    this->socket_address.sin_family = AF_INET;                // Use IPv4 socket family
    this->socket_address.sin_addr.s_addr = INADDR_ANY;        // Listen on any available network interface
    this->socket_address.sin_port = htons(port);              // Set the port number in network byte order
    
    // Step 4: Set socket to non-blocking mode
    // Uses file control function to set properties of a socket
    if (fcntl(main_socket, F_SETFL, O_NONBLOCK) < -1)
    {
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    // Step 5: Bind the socket to the socket_address
    // Step 5.1: Convert the socket_address structure to a pointer of type struct sockaddr *
    struct sockaddr *socket_address = (struct sockaddr *)&this->socket_address;
    
    // Step 5.2: Get the size of the socket_address structure
    socklen_t address_size = sizeof(this->socket_address);
    
    // Step 5.3 Attempt to bind the socket to the socket_address
    if (bind(this->main_socket, socket_address, address_size) < 0)
    {
        throw std::runtime_error("Failed to bind socket");
    }

    // Print listening information
    std::cout << "Listening on port: " << this->port << std::endl;
    std::cout << "The password you chose to use is: " << this->password << std::endl;

    // Step 6: Start listening for incoming connections
    if (listen(this->main_socket, QUEUE_SIZE) == -1)
    {
        throw std::runtime_error("Failed to start listening");
    }

    // Print waiting message if VERBOSE is enabled
    std::cout << "Waiting for connection........" << std::endl << std::endl;
}

// This function accepts incoming connections.

void IRCServer::start()
{
    int socket_descriptor; // fd that is associated with a network socket, used for sending/receiving data, accepting new connections etc.
    int highest_socket_descriptor; // requirement for select()
    (void)highest_socket_descriptor;
    while (TRUE) // Continuously handle client connections..
    {
        socket_descriptor = accept(this->main_socket, NULL, NULL);
        if (socket_descriptor == -1)
        {
            sleep(1);
            std::cout << "\nWaiting..\n" << std::endl;
        }
    }
}

IRCServer::~IRCServer()
{
    // 
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/08/29 10:42:04 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/09/06 18:31:44 by mikuiper      ########   odam.nl         */
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
    this->main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->main_socket == 0) // Use IPv4 socket family and TCP type socket
    {
        throw std::runtime_error("Failed to create socket");
    }
        
    // Step 2: Set socket options to allow reusing the socket_address
    int setsockopt_ret = setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
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
    std::cout << "Waiting for connection.." << std::endl << std::endl;
}

// This function accepts incoming connections.


#include <ctime> // Include the <ctime> header for time-related functions



void IRCServer::start()
{
    int socket_descriptor;
    char buffer[4096];
    
    while (true)
    {
        socket_descriptor = accept(this->main_socket, NULL, NULL);
        if (socket_descriptor == -1)
        {
			time_t raw_datetime = std::time(NULL);
            struct tm datetime;
            localtime_r(&raw_datetime, &datetime);

            char time_buffer[9];
            strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &datetime);

            // [DEBUGGING]: Show that the server is waiting and also show how to connect
            std::cout << "\nWaiting...[" << time_buffer << "]" << std::endl;
            std::cout << "Server IP is: [" << this->IP << "]" << std::endl;
            std::cout << "Server port is: [" << this->port << "]" << std::endl;

            sleep(1);
            continue;
        }
        
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





IRCServer::~IRCServer()
{
    // 
}


/*
irssi
/connect 127.0.1.1 1234
*/
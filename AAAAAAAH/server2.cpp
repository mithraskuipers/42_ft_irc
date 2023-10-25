#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../nextline/get_next_line.hpp"

int main()
{
	char hostbuffer[256];
	struct hostent *host_entry;
	int hostname;
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    host_entry = gethostbyname(hostbuffer);
	printf("IP: %s\n", inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])));
	int servFD;
	int newSock;
	int opt = 1;
	char kapotgroot[2134];
	struct sockaddr_in sockStruct;
	socklen_t adrLen = sizeof(sockStruct);


	// socket creates socket
	servFD = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(servFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	sockStruct.sin_family = AF_INET;
	sockStruct.sin_addr.s_addr = INADDR_ANY;
	sockStruct.sin_port = htons(6667);
	while (1)
	{
		// bind assigns adress to socket
		bind(servFD, (struct sockaddr*)&sockStruct, sizeof(sockStruct));
		
		// listen prepares socket for: accept(socket)
		listen(servFD, 3);

		newSock = accept(servFD, (struct sockaddr*)&sockStruct, &adrLen);
		printf("%s\n", get_next_line(newSock));
	}
}
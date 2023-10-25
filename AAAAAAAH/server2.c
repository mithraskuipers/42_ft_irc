#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	int servFD;
	int newSock;
	int opt = 1;
	char kapotgroot[2134];
	struct sockaddr_in sockStruct;
	struct sockaddr *SSadr = &sockStruct;
	socklen_t adrLen = sizeof(sockStruct);

	servFD = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(servFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	sockStruct.sin_family = AF_INET;
	sockStruct.sin_addr.s_addr = INADDR_ANY;
	sockStruct.sin_port = htons(1111);

	bind(servFD, SSadr, sizeof(sockStruct));
	listen(servFD, 3);
	newSock = accept(servFD, SSadr, &adrLen);
	while (1)
	{
		read(newSock, kapotgroot, sizeof(kapotgroot));
		printf("%s", kapotgroot);
	}
}
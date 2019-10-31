#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "constants.h"

#include<errno.h>

int serv_side_setup (int serv_port) {
	int serv_sock;

	struct sockaddr_in serv_addr;

    if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(0);
	}

	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
		printf("Error sock opt\n");
		exit(0);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(serv_port);

    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)	{
		printf("Error binding socket: %d\n", errno);
		exit(0);
	}

    if (listen(serv_sock, __MAX_PENDING__) < 0) {
		printf("Error listening socket: %d\n", errno);
		exit(0);
	}

	return serv_sock;
}

int clnt_side_setup (char *serv_ip, int serv_port) {
	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	if(serv_ip == NULL)
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_aton(serv_ip, &(serv_addr.sin_addr));
	serv_addr.sin_port = htons(serv_port);

	int con_fd = socket(AF_INET, SOCK_STREAM, 0);

	int connect_res = connect(con_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(connect_res == -1) {
		printf("Error in connecting to client. Exiting...\n");
		exit(0);
	}

	return con_fd;
}

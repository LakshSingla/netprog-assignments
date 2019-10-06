#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*int serv_side_setup (int serv_port) {
	int serv_sock, clnt_sock;

	struct sockaddr_in serv_addr, clnt_addr;

    if ((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Error creating socket\n");
		exit(0);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(serv_port);

    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)	{
		printf("Error binding socket\n");
		exit(0);
	}

    if (listen(serv_sock, MAXPENDING) < 0) {
		printf("Error listening socket\n");
		exit(0);
	}

	for (;;) {
		int clnt_len = sizeof(clnt_addr);

        if ((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0) {
			printf("Error listening socket\n");
			exit(0);
		}

		char buf[MAX_CMD_SIZE];
		size_t numread = read(clnt_sock, buf, MAX_CMD_SIZE);

		if (numread < 0) {
			printf("Error reading from server\n");
			break;
		}

		buf[numread] = '\0';

		//
		// execution
		//

		char output[MAX_RESPONSE_SIZE];
		write(fd, output, MAX_RESPONSE_SIZE); // change size here
	}
}*/

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

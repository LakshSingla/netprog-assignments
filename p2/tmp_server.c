#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;

	struct sockaddr_in serv_addr, clnt_addr;

    if ((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Error creating socket\n");
		exit(0);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(8080);

    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)	{
		printf("Error binding socket\n");
		exit(0);
	}

    if (listen(serv_sock, MAXPENDING) < 0) {
		printf("Error listening socket\n");
		exit(0);
	}

    for (;;)
    {
		int clnt_len = sizeof(clnt_addr);

        if ((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0) {
			printf("Error listening socket\n");
			exit(0);
		}

        printf("Handling client %s\n", inet_ntoa(clnt_addr.sin_addr));

		while (1) {
			sleep(2);
			write(clnt_sock, "Hello world\n", 12);
		}
    }
}


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>

#include "constants.h"
#include "tcp_helpers.h"

int main () {
	// Server setup
	int clnt_sock, serv_sock = serv_side_setup(__NAME_SERVER_PORT__);
	struct sockaddr_in clnt_addr;

	int maxj = -1, maxfd = serv_sock;
	int clients[FD_SETSIZE];
	fd_set rset, allset;

	for (int i = 0; i < FD_SETSIZE; i++) clients[i] = -1;

	FD_ZERO(&allset);
	FD_SET(serv_sock, &allset);

	while (true) {
		rset = allset;

		int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
	
		if (FD_ISSET(serv_sock, &rset)) {
			// new connection
			int clnt_len = sizeof(clnt_addr);
			clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len);

			int j;
			for (j = 0; j < FD_SETSIZE; j++) {
				if (clients[j] == -1) {
					clients[j] = clnt_sock;
					break;
				}
			}
			if (j == FD_SETSIZE) {
				printf("Too many clients. Exiting...\n");
				exit(0);
			}

			FD_SET(clnt_sock, &allset);

			if (j > maxj) maxj = j;
			if (clnt_sock > maxfd) maxfd = clnt_sock;

			nready--;
			if (nready <= 0) continue;
		}

		for (int x = 0; x <= maxj; x++) {
			// check clients
			int csock = clients[x];
			int flags = fcntl(csock, F_GETFL, 0);
			fcntl(csock, F_SETFL, flags | O_NONBLOCK);

			if (csock == -1) continue;

			if (FD_ISSET(csock, &rset)) {
				int nb;
				char buf[1024];
				while (true) {
					nb = read(csock, buf, 1024);

					if (nb < 0) {
						if (errno != EWOULDBLOCK && errno != EAGAIN)
							perror("Error reading from client");
						break;
					}
					else if (nb == 0) {
						close(csock);
						FD_CLR(csock, &allset);
						clients[x] = 0;
						break;
					}
					else {
						buf[nb] = 0;
						printf("Received:\n%s\n", buf);
					}
				}			
			}
		}
	}
}

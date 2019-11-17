#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>

#include "constants.h"
#include "tcp_helpers.h"
#include "fileHierarchyMethods.h"

int file_id = 1;
int maxj, maxfd;
fd_set allrset, allwset, rset, wset;
int clients[FD_SETSIZE];
int clients_status[FD_SETSIZE];
char clients_read_buf[__ONE_MB__][FD_SETSIZE]; // to remember how much has been read 
int clients_read_buf_sz[FD_SETSIZE];
char clients_cmd_code[__CMD_CODE_LEN__ + 1][FD_SETSIZE]; // to remember how much has been read 

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: ./ds.out <port>\n");
		exit(0);
	}
	int my_port = atoi(argv[1]);
	int serv_sock = serv_side_setup(my_port);

	maxj = -1;
	maxfd = serv_sock;

	for (int i = 0; i < FD_SETSIZE; i++) {
		clients[i] = -1;
		clients_status[i] = -1;
	}

	FD_ZERO(&allrset);
	FD_ZERO(&allwset);
	FD_SET(serv_sock, &allrset);

	while (true) {
		rset = allrset;
		wset = allwset;

		int nready = select(maxfd + 1, &rset, &wset, NULL, NULL);

		if (FD_ISSET(serv_sock, &rset)) {
			struct sockaddr_in clnt_addr;
			int clnt_len = sizeof(clnt_addr);
			int clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len);

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
			clients_status[j] = __INIT_STATUS__;

			FD_SET(clnt_sock, &allrset);

			if (j > maxj) maxj = j;
			if (clnt_sock > maxfd) maxfd = clnt_sock;

			nready--;
			if (nready <= 0) continue;
		}

		for (int x = 0; x <= maxj; x++) {
			int csock = clients[x];
			if (csock == -1) continue;

			int flags = fcntl(csock, F_GETFL, 0);
			fcntl(csock, F_SETFL, flags | O_NONBLOCK);

			if (FD_ISSET(csock, &rset)) {
				char buf[__MAX_CMD_SIZE__];
				int nb = read(csock, buf, __MAX_CMD_SIZE__);

				if (nb < 0) {
					if (errno != EWOULDBLOCK && errno != EAGAIN)
						perror("Error reading from client");
					continue;
				}
				else if (nb == 0) {
					close(csock);
					FD_CLR(csock, &allrset);
					clients[x] = -1;
					clients_status[x] = -1;
					break;
				}
				else {
					buf[nb] = 0;

					char data_sz[10];
					memset(data_sz, 10, 0);
					strcpy(data_sz, buf+2);
					int d_sz = atoi(data_sz);

					buf[2] = 0;
					memset(clients_cmd_code[x], __CMD_CODE_LEN__ + 1, 0);
					strcpy(clients_cmd_code[x], buf);

					/*clients_status[x] = __CMD_READ__;*/

					printf("%s; init buf: %d\n", clients_cmd_code[x], d_sz);

					char new_fn[50];
					sprintf(new_fn, "%d", file_id);
					file_id++;

					write(csock, new_fn, strlen(new_fn) * sizeof(char));

					char data[d_sz];
					int nb = read(csock, data, d_sz);
					data[nb] = 0;
					printf("here:\n%s\n", data);

					FD_CLR(csock, &allrset);
					/*FD_SET(csock, &allwset);*/
				}

			}
			else if (FD_ISSET(csock, &wset)) {

			}
		}
	}
}

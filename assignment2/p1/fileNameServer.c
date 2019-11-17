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

char *working_dir[__MAX_PATH_COMPONENTS__] = {NULL};
int working_len = 0;

int maxj, maxfd;
int clients[FD_SETSIZE];
int clients_status[FD_SETSIZE]; // to remember the status of each client
char clients_read_buf[__ONE_MB__][FD_SETSIZE]; // to remember how much has been read 
int clients_read_buf_sz[FD_SETSIZE];
int clients_total_to_read[FD_SETSIZE];
int clients_total_read[FD_SETSIZE];
char clients_cmd_code[__CMD_CODE_LEN__ + 1][FD_SETSIZE]; // to remember how much has been read 
fd_set rset, wset, allset, allwset;

void upload_to_ds (void *x) {
	int i = *((int *) x);
	printf("here\n");
	if (write(2, clients_read_buf[i], clients_read_buf_sz[i]) == -1) {
		perror("Err here");
	}
	printf("\n");

	clients_read_buf_sz[i] = 0;
	memset(clients_read_buf[i], 0, __ONE_MB__);

	return;
}

int main () {
	// Server setup
	int clnt_sock, serv_sock = serv_side_setup(__NAME_SERVER_PORT__);
	struct sockaddr_in clnt_addr;

	maxj = -1;
	maxfd = serv_sock;

	for (int i = 0; i < FD_SETSIZE; i++) {
		clients[i] = -1;
		clients_status[i] = -1;
	}
	FD_ZERO(&allset);
	FD_ZERO(&allwset);
	FD_SET(serv_sock, &allset);

	while (true) {
		rset = allset;
		wset = allwset;

		int nready = select(maxfd + 1, &rset, &wset, NULL, NULL);
	
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
			clients_status[j] = __INIT_STATUS__;

			FD_SET(clnt_sock, &allset);

			if (j > maxj) maxj = j;
			if (clnt_sock > maxfd) maxfd = clnt_sock;

			nready--;
			if (nready <= 0) continue;
		}

		for (int x = 0; x <= maxj; x++) {
			// check clients
			int csock = clients[x];
			if (csock == -1) continue;

			int flags = fcntl(csock, F_GETFL, 0);
			fcntl(csock, F_SETFL, flags | O_NONBLOCK);


			if (FD_ISSET(csock, &rset)) {
				if (clients_status[x] == __INIT_STATUS__) {
					// read cmd
					
					char buf[__MAX_CMD_SIZE__];
					int nb = read(csock, buf, __MAX_CMD_SIZE__);

					if (nb < 0) {
						if (errno != EWOULDBLOCK && errno != EAGAIN)
							perror("Error reading from client");
						continue;
					}
					else if (nb == 0) {
						close(csock);
						FD_CLR(csock, &allset);
						clients[x] = 0;
						clients_status[x] = -1;
						break;
					}
					else {
						buf[nb] = 0;

						memset(clients_read_buf[x], __ONE_MB__, 0);
						strcpy(clients_read_buf[x], buf+2);
						clients_read_buf_sz[x] = strlen(buf+2) * sizeof(char);

						buf[2] = 0;
						memset(clients_cmd_code[x], __CMD_CODE_LEN__ + 1, 0);
						strcpy(clients_cmd_code[x], buf);

						clients_status[x] = __CMD_READ__;

						printf("filename: %s\n", clients_read_buf[x]);

						FD_CLR(csock, &allset);
						FD_SET(csock, &allwset);

					}
					nready--;
				}
				else if (clients_status[x] == __CMD_READ__) {
					// cmd already read
					
					if (strcmp(clients_cmd_code[x],__UPLOAD_CODE__) == 0) {
						clients_status[x] = __UPLOADING_FILE__;
						char data_recv[__ONE_MB__ + 1];
						int nb = read(csock, data_recv, __ONE_MB__);
						data_recv[nb] = 0;

						
						if (nb < 0) {
							if (errno != EWOULDBLOCK && errno != EAGAIN)
								perror("Error reading from client");
							continue;
						}
						else if (nb == 0) {
							close(csock);
							FD_CLR(csock, &allset);
							clients[x] = 0;
							clients_status[x] = -1;
							break;
						}

						strcpy(clients_read_buf[x], strchr(data_recv, '#') + 1);
						char *to_read = strtok(data_recv, "#");
						printf("to_read: %s\n", to_read);
						printf("buf: %s\n", clients_read_buf[x]);
						clients_total_to_read[x] = atoi(to_read);
						clients_total_read[x] = 0;

						clients_read_buf_sz[x] = strlen(clients_read_buf[x]) * sizeof(char); 
						clients_total_read[x] += strlen(clients_read_buf[x]) * sizeof(char);
						printf("%d size: %d\n", x, clients_read_buf_sz[x]);
						pthread_t tid;
						pthread_create(&tid, NULL, upload_to_ds, (void *)&x);
						
						if (pthread_detach(tid) != 0) {
							perror("Error with pthread_detach()");
							exit(0);
						}

						if (clients_total_read[x] >= clients_total_to_read[x]) {
							clients_status[x] = __FILE_UPLOADED__;
							FD_CLR(csock, &allset);
							FD_SET(csock, &allwset);
						}
					}	
				}
				else if (clients_status[x] == __UPLOADING_FILE__) {
					printf("here4\n");
					if (clients_read_buf_sz[x] == 0) {
						printf("here2 %d\n", x);
						if (clients_total_read[x] < clients_total_to_read[x]) {
							printf("here3 %d\n", x);
							int nb = read(csock, clients_read_buf[x], __ONE_MB__);

							if (nb < 0) {
								if (errno != EWOULDBLOCK && errno != EAGAIN)
									perror("Error reading from client");
								continue;
							}
							else if (nb == 0) {
								close(csock);
								FD_CLR(csock, &allset);
								clients[x] = 0;
								clients_status[x] = -1;
								break;
							}
							else {
								clients_read_buf_sz[x] = nb;
								clients_total_read[x] += nb;
								pthread_t tid;
								pthread_create(&tid, NULL, upload_to_ds, &x);
								
								if (pthread_detach(tid) != 0) {
									perror("Error with pthread_detach()");
									exit(0);
								}
							}	
						}
						else {
							clients_status[x] = __FILE_UPLOADED__;
							FD_CLR(csock, &allset);
							FD_SET(csock, &allwset);
						}
					}
				}
				
			}
			else if (FD_ISSET(csock, &wset)) {
				if (clients_status[x] == __CMD_READ__) {
					// writing random char for ack
					write(csock, "a", 1);

					FD_CLR(csock, &allwset);
					FD_SET(csock, &allset);
				}
				else if (clients_status[x] == __FILE_UPLOADED__) {
					char *reply = "File uploaded successfully!";
					write(csock, reply, strlen(reply) * sizeof(char));

					FD_CLR(csock, &allwset);
					FD_SET(csock, &allset);
					clients_status[x] = __INIT_STATUS__;
				}	
			}

			if (nready <= 0) break;
		}
	}
}

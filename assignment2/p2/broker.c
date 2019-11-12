#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "constants.h"
#include "tcp_helpers.h"
#include "broker.h"

/*
 *void parse_config (char *filename, char **self_ip, int *self_port,
 *                char **brok_ip, int *brok_ip) {
 *        if (filename == NULL) {
 *                *self_ip = strdup("127.0.0.1");
 *                *self_port = __DFL_PORT1__;
 *                *brok_ip = strdup("127.0.0.1");
 *
 *        }	
 *}
 */

int main (int argc, char *argv[]) {
	if (argc != 2){
		printf("Incorrect number of arguments.\n");
		exit(0);
	}

	int my_index = atoi(argv[1]);

	char *self_ip = BROKERS[my_index].ip;
	int self_port = BROKERS[my_index].port;

	char *n1_ip = BROKERS[my_index].n1_ip;
	int n1_port = BROKERS[my_index].n1_port;

	char *n2_ip = BROKERS[my_index].n2_ip;
	int n2_port = BROKERS[my_index].n2_port;
	
	int serv_sock = serv_side_setup (self_port); 

/*
 *        if (brok_ip != NULL) {
 *                printf("brok ip: %s\n", brok_ip);
 *                if (fork() == 0) {
 *                        int con_fd = clnt_side_setup(brok_ip, brok_port);
 *                        printf("here: %d\n", con_fd);
 *
 *                        // informing the broker that the connection is from another broker
 *                        int n = write(con_fd, __BROK_CLASS__, __CLASS_LEN__);
 *                        if (n != __CLASS_LEN__) {
 *                                printf("Error with write()\n");
 *                                exit(0);
 *                        }
 *                        
 *                        // receive your left and right neighbours for the topology
 *                        char left_right_brok [2 * INET_ADDRSTRLEN + 1]; // "ip1#ip2"
 *                        int nr = read(con_fd, left_right_brok, 2 * INET_ADDRSTRLEN + 1);
 *                        if (nr <= 0) {
 *                                printf("Error with read()\n");
 *                                exit(0);
 *                        }
 *                        char *tok = strtok(left_right_brok, "#");
 *                        left_broker = strdup(tok);
 *                        tok = strtok(NULL, "#");
 *                        right_broker = strdup(tok);
 *                        printf("left: %s, right: %s\n", left_broker, right_broker);
 *                        exit(1);
 *                }
 *        }
 */

	while (true) {
		struct sockaddr_in clnt_addr;
		int clnt_len = sizeof(clnt_addr);

		int clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len);
		if (clnt_sock < 0) {
			perror("Error in accept()");
			exit(0);
		}

		int ch = fork();

		if (ch == 0) {
			// finding out whether the connection is from
			// subscriber, publisher, or another broker
			
			printf("ehre: %d\n", clnt_sock);
			char conn_class[__CLASS_LEN__];
			int n = read(clnt_sock, conn_class, __CLASS_LEN__);
			if (n != __CLASS_LEN__) {
				printf("Invalid connection class specified from client\n");
				printf("Closing connection...\n");
				close(clnt_sock);
				exit(1);
			}

			if (strcmp(conn_class, __BROK_CLASS__) == 0) {
				printf("broker connection\n");

			}
			else if (strcmp(conn_class, __SUB_CLASS__) == 0) {
				printf("subscriber connection\n");
			} 
			else if (strcmp(conn_class, __PUB_CLASS__) == 0) {
				printf("publisher connection\n");
			} 
			else {
				printf("%s\n", conn_class);
			}
		}
		else if (ch > 0) {

		}
		else {
			perror("fork() error");
			exit(0);
		}
	}
}

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
			
			char conn_class[__CLASS_LEN__];
			int n = read(clnt_sock, conn_class, __CLASS_LEN__);
			if (n != __CLASS_LEN__) {
				printf("Invalid connection class specified from client\n");
				printf("Closing connection...\n");
				close(clnt_sock);
				exit(1);
			}

			printf("here: %s\n", conn_class);
			if (strcmp(conn_class, __BROK_CLASS__) == 0) {
				printf("broker connection\n");
			}
			else if (strcmp(conn_class, __SUB_CLASS__) == 0) {
				printf("subscriber connection\n");
				char cmd_code[1];
				if (read(clnt_sock, cmd_code, 1) != 1) {
					printf("Incorrect command code\n");
					printf("Closing connection...");
					close(clnt_sock);
					exit(0);
				}
				if (cmd_code[0] == '0') {
					printf("subscriber subscribe\n");
				}
				else if (cmd_code[0] == '1') {
					printf("subscriber read\n");
				}
				else if (cmd_code[0] == '2') {
					printf("subscriber read all\n");
				}
			} 
			else if (strcmp(conn_class, __PUB_CLASS__) == 0) {
				printf("publisher connection\n");
				char cmd_code[1];
				if (read(clnt_sock, cmd_code, 1) != 1) {
					printf("Incorrect command code\n");
					printf("Closing connection...");
					close(clnt_sock);
					exit(0);
				}
				if (cmd_code[0] == '0') {
					printf("publisher create\n");
				}
				else if (cmd_code[0] == '1') {
					printf("publisher send\n");
				}
				else if (cmd_code[0] == '2') {
					printf("publisher send file\n");
				}
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

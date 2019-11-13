#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "constants.h"
#include "tcp_helpers.h"
#include "broker.h"
#include "broker_sub_methods.h"

struct broker BROKERS[3] = {
	{"127.0.0.1", 4000, "127.0.0.1", 5000, "127.0.0.1", 6000},
	{"127.0.0.1", 5000, "127.0.0.1", 4000, "127.0.0.1", 6000},
	{"127.0.0.1", 6000, "127.0.0.1", 5000, "127.0.0.1", 4000}
};

int *curr_topic_count;
struct topic_msg_list **MAIN_TOPIC_LIST;
int *topic;

int main (int argc, char *argv[]) {
	if (argc != 2){
		printf("Incorrect number of arguments.\n");
		exit(0);
	}

	int my_index = atoi(argv[1]);

	// initialize self and neighbouring ips
	char *self_ip = BROKERS[my_index].ip;
	int self_port = BROKERS[my_index].port;

	char *n1_ip = BROKERS[my_index].n1_ip;
	int n1_port = BROKERS[my_index].n1_port;

	char *n2_ip = BROKERS[my_index].n2_ip;
	int n2_port = BROKERS[my_index].n2_port;
	
	// Shared memory for curr_topic_count creation
	int shmkey1 = ftok("./broker.c", 1);
	int shm1 = shmget (shmkey1, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (shm1 == -1) {
		perror("Error with shmget()");
		exit(0);
	}

	curr_topic_count = shmat(shm1, NULL, 0);
	if (curr_topic_count == (void *) -1) {
		perror("Error with shmat()");
		exit(0);
	}

	// Shared memory for MAIN_TOPIC_LIST
/*
 *        int shmkey2 = ftok("./broker.c", 2);
 *        int shm2 = shmget (shmkey2, sizeof(struct topic_msg_list *) * __MAX_TOPIC_COUNT__, 
 *                        IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
 *        if (shm2 == -1) {
 *                perror("Error with shmget()");
 *                exit(0);
 *        }
 *
 *        MAIN_TOPIC_LIST = shmat(shm2, NULL, 0);
 *        if (MAIN_TOPIC_LIST == (void *) -1) {
 *                perror("Error with shmat()");
 *                exit(0);
 *        }
 */

	// Shared memory for
	int shmkey2 = ftok("./broker.c", 2);
	int shm2 = shmget (shmkey2, sizeof(int) * __MAX_TOPIC_COUNT__, 
			IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (shm2 == -1) {
		perror("Error with shmget()");
		exit(0);
	}

	topic = (int *) shmat(shm2, NULL, 0);
	if (topic == (void *) -1) {
		perror("Error with shmat()");
		exit(0);
	}


	*curr_topic_count = 0;

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

					char topic[__MAX_TOPIC_SIZE__];
					int nt = read(clnt_sock, topic, __MAX_TOPIC_SIZE__);
					if (nt <= 0) {
						printf("Error reading topic from publisher\n");
						printf("Closing Connection...");
						close(clnt_sock);
						exit(0);
					}
					printf("topic: %s\n", topic);
					handle_topic_create (clnt_sock, topic);
					/*printf("created: %s\n", MAIN_TOPIC_LIST[*curr_topic_count-1]->topic);*/
					printf("created %d: %d\n", *curr_topic_count - 1, topic[*curr_topic_count-1]);
					close(clnt_sock);
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

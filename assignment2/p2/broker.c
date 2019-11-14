#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
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

// Lock sem before it
void update_shared_memory(struct shared_mem_structure *addr, const char *name) {
	int count = addr->n;
	for(int i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, name) == 0) return;
	}
	strcpy((addr->lt[count]).topic_name, name);
	(addr->lt[count]).no_messages = 0;
	addr->n = count+1;
	return;
}

void add_message(struct shared_mem_structure *addr, const char *name, const char *msg) {
	
	int count = addr->n;
	for(int i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, name) == 0) {
			strcpy((addr->lt[i]).msg_arr[(addr->lt[i]).no_messages], msg);
			(addr->lt[i]).no_messages += 1;
			return;
		}
	}
}

// Unlock sem before it

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

	// Shared memory for
	int shmkey2 = ftok("./broker.h", 2);
	int shm2 = shmget (shmkey2, sizeof(int) * __MAX_TOPIC_COUNT__, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (shm2 == -1) {
		perror("Error with shmget()");
		exit(0);
	}

	topic = (int *) shmat(shm2, NULL, 0);
	if (topic == (void *) -1) {
		perror("Error with shmat()");
		exit(0);
	}

	int shmkey3 = ftok("./broker.c", 3);
	printf("%ld\n", sizeof(struct shared_mem_structure));
	int shm3 = shmget (shmkey3, sizeof(struct shared_mem_structure), IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(shm3 == -1) {
		perror("Error with shmget()");
		exit(0);
	}

	struct shared_mem_structure *sh_mem = shmat(shm3, NULL, 0);
	sh_mem->n = 0;

	printf("%d\n", sh_mem->n);
	if (sh_mem == (void *) -1) {
		perror("Error with shmat()");
		exit(0);
	}

	/*if(fork() == 0) {
		update_shared_memory(sh_mem, "robin");
		exit(0);
	}
	wait(NULL);
	if(fork() == 0) {
		update_shared_memory(sh_mem, "robin2");
		exit(0);
	}
	wait(NULL);
	if(fork() == 0) {
		update_shared_memory(sh_mem, "robin");
		exit(0);
	}
	wait(NULL);	

	add_message(sh_mem, "robin", "msg1");
	add_message(sh_mem, "robin2", "msg1");
	add_message(sh_mem, "robin", "msg1");
	add_message(sh_mem, "robin", "msg2");
	if(fork() == 0) {
		add_message(sh_mem, "robin2", "x");
		exit(0);
	}

	wait(NULL);

	printf("%d\n", sh_mem->n);
	for(int i = 0; i < sh_mem->n; ++i) {
		printf("%s\n", (sh_mem->lt[i]).topic_name);	
		for(int j = 0; j < (sh_mem->lt[i]).no_messages; ++j) {
			printf("%s\n", (sh_mem->lt[i]).msg_arr[j]);	
		}
	}

	*curr_topic_count = 0;
*/
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
					topic[nt] = 0;
					if (nt <= 0) {
						printf("Error reading topic from publisher\n");
						printf("Closing Connection...");
						close(clnt_sock);
						exit(0);
					}
					printf("topic: %s\n", topic);
					handle_topic_create (clnt_sock, topic, sh_mem);
					/*printf("created: %s\n", MAIN_TOPIC_LIST[*curr_topic_count-1]->topic);*/
					printf("created %d: %d\n", sh_mem->n, topic[sh_mem->n - 1]);
					close(clnt_sock);
				}
				else if (cmd_code[0] == '1') {
					printf("publisher send\n");

					char topic_n_msg[__MAX_TOPIC_SIZE__ + __MAX_MSG_SIZE__];
					int nm = read(clnt_sock, topic_n_msg, __MAX_TOPIC_SIZE__ + __MAX_MSG_SIZE__);
					topic_n_msg[nm] = 0;
					if (nm <= 0) {
						printf("Error reading message from publisher\n");
						printf("Closing Connection...");
						close(clnt_sock);
						exit(0);
					}
					printf("%d here: %s\n", nm, topic_n_msg);

					char *topic_n_msg_copy = strdup(topic_n_msg);
					char *tok = strtok(topic_n_msg, "#");
					char *topic = strdup(tok);
					char *msg = strchr(topic_n_msg_copy, '#') + 1;
					handle_msg_recv(clnt_sock, topic, msg, sh_mem);
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

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
#include <time.h>
#include "constants.h"
#include "tcp_helpers.h"
#include "broker.h"
#include "broker_sub_methods.h"
#include "broker_pub_methods.h"
#include "broker_methods.h"
#include "common_utils.h"

// Lock sem before it
/*
 *void update_shared_memory(struct shared_mem_structure *addr, const char *name) {
 *        int count = addr->n;
 *        for(int i = 0; i < count; ++i) {
 *                if(strcmp((addr->lt[i]).topic_name, name) == 0) return;
 *        }
 *        strcpy((addr->lt[count]).topic_name, name);
 *        (addr->lt[count]).no_messages = 0;
 *        addr->n = count+1;
 *        return;
 *}
 *
 *void add_message(struct shared_mem_structure *addr, const char *name, const char *msg) {
 *        
 *        int count = addr->n;
 *        for(int i = 0; i < count; ++i) {
 *                if(strcmp((addr->lt[i]).topic_name, name) == 0) {
 *                        strcpy((addr->lt[i]).msg_arr[(addr->lt[i]).no_messages], msg);
 *                        (addr->lt[i]).no_messages += 1;
 *                        return;
 *                }
 *        }
 *}
 *
 */
// Unlock sem before it

struct broker BROKERS[3] = {
	{"127.0.0.1", 4000, "127.0.0.1", 5000, "127.0.0.1", 6000},
	{"127.0.0.1", 5000, "127.0.0.1", 6000, "127.0.0.1", 5000},
	{"127.0.0.1", 6000, "127.0.0.1", 4000, "127.0.0.1", 5000}
};

char *self_ip;
int self_port;

char *left_ip;
int left_port;

char *right_ip;
int right_port;
	

/*
 *void del_msg () {
 *        printf("here: %s\n", sh_mem->lt[count].msg_array)
 *}
 */

int main (int argc, char *argv[]) {
	if (argc != 2){
		printf("Incorrect number of arguments.\n");
		exit(0);
	}

	int my_index = atoi(argv[1]);

	// initialize self and neighbouring ips
	self_ip = BROKERS[my_index].ip;
	self_port = BROKERS[my_index].port;

	left_ip = BROKERS[my_index].left_ip;
	left_port = BROKERS[my_index].left_port;

	right_ip = BROKERS[my_index].right_ip;
	right_port = BROKERS[my_index].right_port;
	
	srand(time(NULL));
	int shmkey3 = ftok("./broker.c", rand() * getpid());
	printf("%d %ld\n", shmkey3, sizeof(struct shared_mem_structure));
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
	/*signal(SIGALRM, del_msg);*/
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

				char topic[__MAX_TOPIC_SIZE__];
				read_rem_msg(clnt_sock, topic, __MAX_MSG_SIZE__);
				handle_brok (clnt_sock, topic, sh_mem);
				close(clnt_sock);
				printf("retrieve: %s\n", topic);
			}
			else if (strcmp(conn_class, __SUB_CLASS__) == 0) {
				printf("subscriber connection\n");
				char cmd_code[1];
				printf("here1\n");
				if (read(clnt_sock, cmd_code, 1) != 1) {
					printf("Incorrect command code\n");
					printf("Closing connection...");
					close(clnt_sock);
					exit(0);
				}
				printf("here2\n");
				if (cmd_code[0] == '0') {
					printf("subscriber subscribe\n");
				}
				else if (cmd_code[0] == '1') {
					printf("subscriber read\n");

					char topic[__MAX_TOPIC_SIZE__];
					read_rem_msg(clnt_sock, topic, __MAX_MSG_SIZE__);
					printf("retrieve: %s\n", topic);
					handle_topic_read(clnt_sock, topic, sh_mem);
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
					read_rem_msg(clnt_sock, topic, __MAX_TOPIC_SIZE__);
					
					printf("topic: %s\n", topic);
					handle_topic_create (clnt_sock, topic, sh_mem);
					close(clnt_sock);
				}
				else if (cmd_code[0] == '1') {
					printf("publisher send\n");

					char topic_n_msg[__MAX_TOPIC_SIZE__ + __MAX_MSG_SIZE__];
					read_rem_msg(clnt_sock, topic_n_msg, __MAX_TOPIC_SIZE__ + __MAX_MSG_SIZE__);

					char *topic_n_msg_copy = strdup(topic_n_msg);
					char *tok = strtok(topic_n_msg, "#");
					char *topic = strdup(tok);
					char *msg = strchr(topic_n_msg_copy, '#') + 1;
					handle_msg_recv(clnt_sock, topic, msg, sh_mem);
					close(clnt_sock);
				}
			} 
			else {
				printf("%s\n", conn_class);
			}
			exit(1);
		}
		else if (ch > 0) {

			close(clnt_sock);
		}
		else {
			perror("fork() error");
			exit(0);
		}
	}
}

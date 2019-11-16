#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "broker.h"
#include "tcp_helpers.h"

char *left_ip;
int left_port;
char *right_ip;
int right_port;

int query_neighbour (char *topic, struct shared_mem_structure *addr) {
	printf("left: %s %d\n", left_ip, left_port);

	int clnt_sock = clnt_side_setup(left_ip, left_port);

	char brok_req[__MAX_CMD_SIZE__];
	int brok_req_size = 4 + strlen(topic);
	strcpy(brok_req, __BROK_CLASS__);
	brok_req[3] = 1;
	brok_req[4] = 0;
	strcat(brok_req, topic);
	brok_req[3] = 0;
	write(clnt_sock, brok_req, brok_req_size);

	char msg_recv_arr[__MAX_MSG_SIZE__ + 1][__BULK_LIMIT__];
	int msg_count = 0;
	while (true) {
		printf("here\n");
		if (msg_count == 10) break;
		char msg_size_str[3];
		int n = read(clnt_sock, msg_size_str, 3);
		if (n <= 0) {
			break;
		}
		int msg_size = atoi(msg_size_str);

		char msg[msg_size + 1];
		int nr = read(clnt_sock, msg, msg_size);

		if (nr <= 0) {
			perror("Error in reading from other broker");
			break;
		}
		else {
			msg[nr] = 0;
			strcpy(msg_recv_arr[msg_count], msg);
			msg_count++;
		}
	}

	if (msg_count > 0) {
		int count = addr->n;
		for(int i = 0; i < count; i++) {
			printf("here1\n");
			if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
				for (int j = 0; j < msg_count; j++) {
					printf("here2\n");
					if ((addr->lt[i]).no_messages == __MAX_MSG_COUNT__) {
						// msg_arr full
						break;
					}
					strcpy((addr->lt[i]).msg_arr[(addr->lt[i]).set_index].msg, msg_recv_arr[j]);
					(addr->lt[i]).msg_arr[(addr->lt[i]).set_index].id = (addr->lt[i]).max_msg_id + 1;
					(addr->lt[i]).no_messages += 1;
					(addr->lt[i]).max_msg_id += 1;
					(addr->lt[i]).set_index = ((addr->lt[i]).set_index + 1) % __MAX_MSG_COUNT__;
				}
				break;
			}
		}
		return 0;
	}
	return -1;
} 

void handle_brok (int fd, char *topic, struct shared_mem_structure *addr) {
	int count = addr->n;
	int i;
	char *reply;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			// topic present
			int msg_count = addr->lt[i].no_messages;
			if (msg_count > 0) {
				struct msg_struct *msg_arr = (addr->lt[i]).msg_arr;

				char resp[__MAX_BROK_RESP_SIZE__];
				memset(resp, 0, __MAX_BROK_RESP_SIZE__);
				for (int j = 0; j < __BULK_LIMIT__; j++) {
					if (j == msg_count) break;

					struct msg_struct curr_msg = msg_arr[j];
					char append_resp[__MAX_RESPONSE_SIZE__];
					// first 3 characters contain the size of the msg
					sprintf(append_resp, "%3ld%s", strlen(curr_msg.msg) * sizeof(char), curr_msg.msg);
					strcat(resp, append_resp);	
				}
				write(fd, resp, strlen(resp) * sizeof(char));
				close(fd);
			}
			else {
				// communicate to other brokers to get messages	
				/*query_neighbour(topic, addr);*/
				close(fd);
			}
			break;
		}
	}
	if (i == count) {
		// topic not present
		close(fd);
	}

	/*write(fd, reply, (strlen(reply) + 1) * sizeof(char));*/
}

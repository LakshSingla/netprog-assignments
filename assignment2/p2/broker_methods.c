#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "broker.h"
#include "tcp_helpers.h"
#include "common_utils.h"

char *left_ip;
int left_port;
char *right_ip;
int right_port;

int query_neighbour (char *topic, char *ip, int port, struct shared_mem_structure *addr) {
	printf("left: %s %d\n", left_ip, left_port);

	if (strcmp(ip, left_ip) == 0 && port == left_port) {
		printf("End of ring!\n");
		return -1;
	}

	int clnt_sock = clnt_side_setup(left_ip, left_port);

	// msg = ip + '#' + port + '#' + topic
	char msg_content[INET_ADDRSTRLEN + 1 + 6 + 1 + __MAX_TOPIC_SIZE__];
	sprintf(msg_content, "%s#%d#%s", ip, port, topic);

	// complete request string
	char brok_req[__MAX_CMD_SIZE__];
	int brok_req_size = 4 + 1 + strlen(msg_content);
	memset(brok_req, 0, brok_req_size);
	strcpy(brok_req, __BROK_CLASS__);
	msg_prefix (brok_req, __BROK_CLASS__, __BROK_RET_CODE__, msg_content);
	brok_req[3] = 0;
	write(clnt_sock, brok_req, brok_req_size);
	
	char msg_recv_arr[__MAX_MSG_SIZE__ + 1][__BULK_LIMIT__];
	int msg_count = 0;
	while (true) {
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
	close(clnt_sock);

	if (msg_count > 0) {
		int count = addr->n;
		for(int i = 0; i < count; i++) {
			if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
				for (int j = 0; j < msg_count; j++) {
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

void handle_brok (int fd, char *msg, struct shared_mem_structure *addr) {
	char *topic = strdup(strchr(strchr(msg, '#') + 1, '#') + 1);
	char *tok = strtok(msg, "#");
	char *ip = strdup(tok);
	tok = strtok(NULL, "#");
	int port = atoi(tok);

	int count = addr->n;
	int i;
	char *reply;
	int msg_count = 0;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			// topic present
			msg_count = addr->lt[i].no_messages;
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
			break;
		}
	}
	if (i == count || msg_count == 0) {
		// topic not present
		// or no messages in topic
		
		// communicate to other brokers to get messages	
		if (query_neighbour(topic, ip, port, addr) == 0) {
			msg_count = addr->lt[i].no_messages;
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
			}
		}
		close(fd);
	}

	/*write(fd, reply, (strlen(reply) + 1) * sizeof(char));*/
}

void inform_topic (char *topic) {
	int clnt_sock = clnt_side_setup(left_ip, left_port);

	char brok_req[__MAX_CMD_SIZE__];
	int brok_req_size = 4 + 1 + strlen(topic);
	memset(brok_req, 0, brok_req_size);
	strcpy(brok_req, __BROK_CLASS__);
	msg_prefix (brok_req, __BROK_CLASS__, __BROK_TOP_CODE__, topic);
	brok_req[3] = 0;
	write(clnt_sock, brok_req, brok_req_size);
	close(clnt_sock);
}

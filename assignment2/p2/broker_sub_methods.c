#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "broker.h"
#include "broker_methods.h"

char *self_ip;
int self_port;

void handle_topic_read (int fd, char *topic, int last_msg_id, struct shared_mem_structure *addr) {
	int count = addr->n;
	int i;
	char *reply;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			// topic present
			int msg_count = addr->lt[i].no_messages;
			if (msg_count > 0) {
				int msg_index = (addr->lt[i]).get_index;
				struct msg_struct ret_msg = (addr->lt[i]).msg_arr[msg_index];
				
				int j = 0;
				while (ret_msg.id <= last_msg_id && j < msg_count) {
					msg_index++;
					j++;
					ret_msg = (addr->lt[i]).msg_arr[msg_index];
				}
				
				if (j == msg_count) reply = "No messages remaining to read!";
				else {
					// need to send msg with id
					char msg_with_id[__MAX_RESPONSE_SIZE__];
					sprintf(msg_with_id, "%d#%s", ret_msg.id, ret_msg.msg);
					write(fd, msg_with_id, strlen(msg_with_id) * sizeof(char));
					return;
				}
			}
			else {
				// communicate to other brokers to get messages	
				if (query_neighbour(topic, self_ip, self_port, addr) != 0) {
					reply = "No messages found for the topic!";	
				}
				else {
					struct msg_struct ret_msg = (addr->lt[i]).msg_arr[(addr->lt[i]).get_index];
					printf("sending: %s\n", ret_msg.msg);
					// need to send msg with id
					char msg_with_id[__MAX_RESPONSE_SIZE__];
					sprintf(msg_with_id, "%d#%s", ret_msg.id, ret_msg.msg);
					write(fd, msg_with_id, strlen(msg_with_id) * sizeof(char));
					return;
				}
			}
			break;
		}
	}
	if (i == count) {
		// topic not present
		reply = "Topic not found!";	
	}

	write(fd, reply, (strlen(reply) + 1) * sizeof(char));
}

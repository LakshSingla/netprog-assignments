#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "broker.h"
#include "constants.h"

int *curr_topic_count;
struct topic_msg_list **MAIN_TOPIC_LIST;
int *topic;

void handle_topic_create (int fd, char *topic, struct shared_mem_structure *addr) {
	char *reply;
	int count = addr->n;
	int i;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			reply = "Topic already exists!";
			break;
		}
	}

	if (i == count) {
		if (count == __MAX_TOPIC_COUNT__) {
			reply = "Max topic count reached!";
		}
		else {
			strcpy((addr->lt[count]).topic_name, topic);
			(addr->lt[count]).no_messages = 0;
			(addr->lt[count]).max_msg_id = 0;
			(addr->lt[count]).get_index = 0;
			(addr->lt[count]).set_index = 0;
			addr->n = count+1;
			reply = "Topic successfully created!";
		}
	}

	write(fd, reply, sizeof(char) * (strlen(reply) + 1));
}

void handle_msg_recv (int fd, char *topic, char *msg, struct shared_mem_structure *addr) {
	printf("topic: %s\nmsg: %s\n", topic, msg);
	char *reply;
	int count = addr->n;
	int i;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			if ((addr->lt[i]).no_messages == __MAX_MSG_COUNT__) {
				// msg_arr full
				reply = "Message limit reached!";
			}
			else {
				strcpy((addr->lt[i]).msg_arr[(addr->lt[i]).set_index].msg, msg);
				(addr->lt[i]).msg_arr[(addr->lt[i]).set_index].id = (addr->lt[i]).max_msg_id + 1;
				(addr->lt[i]).no_messages += 1;
				(addr->lt[i]).max_msg_id += 1;
				(addr->lt[i]).set_index = ((addr->lt[i]).set_index + 1) % __MAX_MSG_COUNT__;
				reply = "Message sent!";
			}
			break;
		}
	}

	if (i == count) {
		reply = "Topic not found!";
	}
	write(fd, reply, sizeof(char) * (strlen(reply) + 1));
	
	if (i != count) {
		sleep(__MSG_TIME_LIMIT__);
		count = addr->n;
		for(i = 0; i < count; ++i) {
			if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
				int msg_count = addr->lt[i].no_messages;
				(addr->lt[i]).get_index = ((addr->lt[i]).get_index + 1) % __MAX_MSG_COUNT__;
				(addr->lt[i]).no_messages -= 1;
				break;
			}
		}
		printf("Deleted!\n");
	}
}

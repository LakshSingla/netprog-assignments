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
			strcpy((addr->lt[i]).msg_arr[(addr->lt[i]).no_messages].msg, msg);
			(addr->lt[i]).no_messages += 1;
			reply = "Message sent!";
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
				for (int j = 0; j < msg_count - 1; j++) {
					addr->lt[i].msg_arr[j] = addr->lt[i].msg_arr[j+1];
				}

				(addr->lt[i]).no_messages -= 1;
				break;
			}
		}
		printf("Deleted!\n");
	}
}

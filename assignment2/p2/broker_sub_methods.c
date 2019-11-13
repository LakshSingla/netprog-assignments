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

void handle_msg_recv (int clnt_sock, char *topic, char *msg) {
	printf("topic: %s\nmsg: %s\n", topic, msg);

}

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "broker.h"
#include "constants.h"

struct topic_msg_list MAIN_TOPIC_LIST[__MAX_TOPIC_COUNT__];

void handle_topic_create (int fd, char *topic) {
	int i = 0;
	char *reply;

	while (i < curr_topic_count) {
		if (strcmp(MAIN_TOPIC_LIST[i].topic, topic) == 0) {
			reply = "Topic already exists!";
		}
	}

	if (i == curr_topic_count) {
		if (curr_topic_count == __MAX_TOPIC_COUNT__) {
			reply = "Max topic count reached!";
		}
		else {
			MAIN_TOPIC_LIST[curr_topic_count].topic = (char *) malloc(strlen(topic) * sizeof(char));
			strcpy(MAIN_TOPIC_LIST[curr_topic_count].topic, topic);
			reply = "Topic successfully craeted!";
		}
	}

	write(fd, reply, sizeof(char) * strlen(reply));
}

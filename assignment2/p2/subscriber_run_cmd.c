#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "subscriber.h"

void run_cmd (char *cmd_buf) {
	char *cmd_copy = strdup(cmd_buf);

	char *tok = strtok(cmd_copy, " ");
	char *cmd = strdup(tok);
	
	tok = strtok(NULL, " ");

	char *topic = strdup(tok);

	if (strcmp(cmd, __SUB_CMD__) == 0) {
		// subscribe to a topic
		printf("here\n");
		for(int i = 0; i < subscribed_topics_len; ++i) {
			if(strcmp(subscribed_topics[i], topic) == 0) {
				printf("Topic already subscribed\n");
			}
		}
		subscribed_topics[subscribed_topics_len] = topic;
		subscribed_topics_len++;
	}
	else if (strcmp(cmd, __SUB_RET_CMD__)) {

	}
	else if (strcmp(cmd, __SUB_RETALL_CMD__)) {

	}
	else {
		printf("Command not found\n");
	}
}

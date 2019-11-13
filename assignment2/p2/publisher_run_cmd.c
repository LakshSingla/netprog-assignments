#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "publisher_methods.h"

void run_cmd (char *cmd_buf, char *broker_ip, int broker_port) {
	char *cmd_copy = strdup(cmd_buf);

	char *tok = strtok(cmd_copy, " ");
	char *cmd = strdup(tok);
	
	tok = strtok(NULL, " ");

	char *topic = strdup(tok);

	if (strcmp(cmd, __PUB_CREATE_CMD__) == 0) {
		// create topic
		create_topic(topic, broker_ip, broker_port);
	}
	else if (strcmp(cmd, __PUB_SEND_CMD__) == 0) {
		// send message to a topic

	}
	else if (strcmp(cmd, __PUB_SENDFILE_CMD__) == 0) {
		// send file as a message to a topic

	}
	else {
		printf("Command not found\n");
	}
}

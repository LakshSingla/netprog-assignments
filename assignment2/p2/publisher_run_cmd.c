#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "tcp_helpers.h"
#include "publisher_methods.h"

void run_cmd (char *cmd_buf, char *broker_ip, int broker_port) {
	char *cmd_copy = strdup(cmd_buf);

	char *tok = strtok(cmd_copy, " ");
	char *cmd = strdup(tok);
	
	tok = strtok(NULL, " ");

	char *topic = strdup(tok);

	int con_fd = clnt_side_setup(broker_ip, broker_port);
	if (strcmp(cmd, __PUB_CREATE_CMD__) == 0) {
		// create topic
		create_topic(topic, con_fd);
	}
	else if (strcmp(cmd, __PUB_SEND_CMD__) == 0) {
		// send message to a topic
		
		tok = strtok(NULL, " ");
		char *msg = strdup(tok);
		send_msg (topic, msg, con_fd);
	}
	else if (strcmp(cmd, __PUB_SENDFILE_CMD__) == 0) {
		// send file as a message to a topic

	}
	else {
		printf("Command not found\n");
	}
}

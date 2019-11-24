#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "constants.h"
#include "subscriber.h"
#include "subscriber_methods.h"
#include "tcp_helpers.h"

int run_cmd (char *cmd_buf, char *broker_ip, int broker_port) {
	char *cmd_copy = strdup(cmd_buf);

	char *tok = strtok(cmd_copy, " ");
	char *cmd = strdup(tok);
	
	tok = strtok(NULL, " ");

	char *topic = strdup(tok);

	if (strcmp(cmd, __SUB_CMD__) == 0) {
		// subscribe to a topic
		subscribe_topic(topic);
		return 0;
	}
	else if (strcmp(cmd, __SUB_RET_CMD__) == 0) {
		int con_fd = clnt_side_setup(broker_ip, broker_port);
		int x = ret_topic(topic, con_fd);
		close(con_fd);
		return x;
	}
	else if (strcmp(cmd, __SUB_RETALL_CMD__) == 0) {
		/*int con_fd = clnt_side_setup(broker_ip, broker_port);*/
		retall_topic(topic, broker_ip, broker_port);
		/*close(con_fd);*/
		return 0;
	}
	else {
		printf("Command not found\n");
		return 0;
	}
}

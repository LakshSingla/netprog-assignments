#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"

void run_cmd (char *cmd_buf) {
	char *cmd_copy = strdup(cmd_buf);

	char *tok = strtok(cmd_copy, " ");
	char *cmd = strdup(tok);
	
	tok = strtok(NULL, " ");

	char *topic = strdup(tok);

	if (strcmp(cmd, __PUB_CREATE_CMD__)) {

	}
	else if (strcmp(cmd, __PUB_SEND_CMD__)) {

	}
	else if (strcmp(cmd, __PUB_SENDFILE_CMD__)) {

	}
	else {
		printf("Command not found\n");
	}
}

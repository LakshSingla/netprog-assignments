#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tcp_helpers.h"
#include "constants.h"
#include "publisher_run_cmd.h"

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf("Incorrect number of arguments.\n");
		printf("Usage: ./a.out <broker_ip>\n");
	}
	char *broker_ip = argv[1];
	
	printf("Commands:\n");
	printf("-> %s <topic>\n", __PUB_CREATE_CMD__);
	printf("-> %s <topic> <msg>\n", __PUB_SEND_CMD__);
	printf("-> %s <topic> <file>\n", __PUB_SENDFILE_CMD__);
	printf("\n\n");
	while (true) {
		printf(__PROMPT__);
		size_t cmd_size = __MAX_CMD_SIZE__+1;
		char *cmd_buf = malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act  == -1 || cmd_size_act == 0 || cmd_size_act == 1 
				&& cmd_buf[0] == '\n') continue;
		cmd_buf[cmd_size_act-1] = 0;



		printf("%s\n", cmd_buf);
		
		/*int broker_sock_fd = clnt_side_setup(broker_ip, __DFL_PORT__);*/
		run_cmd (cmd_buf);
	}
}

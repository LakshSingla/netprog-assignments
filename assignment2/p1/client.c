#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "constants.h"
#include "tcp_helpers.h"
#include "upload_file.h"

int main() {
	int confd = clnt_side_setup(NULL, __NAME_SERVER_PORT__);
	while(true) {
		printf(__PROMPT__);	
		size_t cmd_size = __MAX_CMD_SIZE__+1;
		char *cmd_buf = malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act  == -1 || cmd_size_act == 0 || cmd_size_act == 1 && cmd_buf[0] == '\n') continue;
		cmd_buf[cmd_size_act-1] = 0;



		printf("%s\n", cmd_buf);

		upload_file("bigfile.txt", confd);
	}
}

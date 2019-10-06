#include "clustershell_server.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "constants.h"
#include "parser.h"
#include "read_config.h"

int main(int argc, char **argv) {

	// Setup server
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;

	if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket\n");
		exit(0);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(__SERVER_PORT__);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)	{
		printf("Error binding socket\n");
		exit(0);
	}

	if (listen(serv_sock, __MAX_PENDING__) < 0) {
		printf("Error listening socket\n");
		exit(0);
	}
	// Server in listening mode
	
	char **config_ips = read_config(__CONFIG_FILE__);
	

  while(true) {
		int clnt_len = sizeof(clnt_addr);

		if ((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0) {
			printf("Error listening socket\n");
			exit(0);
		}

		printf("Handling client %s\n", inet_ntoa(clnt_addr.sin_addr));

		pid_t ch_handler = fork();

		if(ch_handler < 0) {
			printf("Error handling the forking. Exiting...\n");	
			exit(0);
		}

		else if(ch_handler == 0) {
			close(serv_sock);
			char cmd_buf[__MAX_CMD_SIZE__];
			int n = read(clnt_sock, cmd_buf, __MAX_CMD_SIZE__);
			if(n < 0) exit(-1);

			PARSE_OBJ pcmd = parse(cmd_buf);
			int cmd_it = 0;
			while(!PARSE_EMPTY(pcmd, cmd_it)) {
				char *cl = PARSE_GET_KEY(pcmd, cmd_it);
				char *in_cmd = PARSE_GET_VAL(pcmd, cmd_it);
				if(cl == '-') {
						
				}
				else if(cl[1] == '*') {
				
				}
				else {
					int cl_no = atoi(cl + 1);	
					int `
				}



				++cmd_it;
			}

			close(clnt_sock);
		}

		close(clnt_sock);

		/*while(true) {
			//sleep(2);
			//write(clnt_sock, "Hello world\n", 12);
		}*/
  }

	CONFIG_FREE(config_ips);
}

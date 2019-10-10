#include "clustershell_server.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include<errno.h>

#include "constants.h"
#include "parser.h"
#include "read_config.h"
#include "tcp_helpers.h"

int main(int argc, char **argv) {

	// Setup server
	int clnt_sock, serv_sock = serv_side_setup(__SERVER_PORT__);
	struct sockaddr_in clnt_addr;

	// Server in listening mode
	char **config_ips = read_config(__CONFIG_FILE__);

  while(true) {
		int clnt_len = sizeof(clnt_addr);

		if ((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0) {
			printf("Error listening socket: %d\n", errno);
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

			while (true) {
				char cmd_buf[__MAX_CMD_SIZE__];
				int n = read(clnt_sock, cmd_buf, __MAX_CMD_SIZE__);
				if(n < 0) { break; exit(-1); }
				cmd_buf[n] = '\0';
				printf("Received command: %s\n", cmd_buf);


				if(strcmp(cmd_buf, __NODES_CMD__) == 0) {
					printf("Nodes command recieved\n");	
					FILE *fp = fopen(__CONFIG_FILE__, "r");
					if(fseek(fp, 0L, SEEK_END) != 0) {
					
					}
					long buf_size = ftell(fp);
					if(buf_size == -1) {}
					char *src = malloc(sizeof(char) * (buf_size+1));

					if(fseek(fp, 0L, SEEK_SET) != 0) {}

					size_t new_len = fread(src, sizeof(char), buf_size, fp);
					src[new_len] = 0;
					++new_len;
					write(clnt_sock, src, new_len);
					free(src);
					continue;
				}

				PARSE_OBJ pcmd = parse(cmd_buf);
				int cmd_it = 0;
				char input_buf[__MAX_OUT_SIZE__ + 1] = {0};
				int input_buf_size = 0;
				while(!PARSE_EMPTY(pcmd, cmd_it)) {
					char *cl = PARSE_GET_KEY(pcmd, cmd_it);
					char *in_cmd = PARSE_GET_VAL(pcmd, cmd_it);

					if(cl[1] == '*') {
						printf("Handling wild\n");
						char **cur_ip = config_ips;
						char concat_resp[__MAX_OUT_SIZE__+1] = {0};
						memset(concat_resp, 0, __MAX_OUT_SIZE__ + 1 );
						int concat_offset = 0;
						while(*cur_ip) {
							printf("XXX\n");
							int con_fd = clnt_side_setup(*cur_ip, __CLIENT_PORT__);				
							char response[__MAX_CMD_SIZE__ + 1 + __MAX_OUT_SIZE__ + 1] = {0};	
							memset(response, 0, __MAX_CMD_SIZE__ + 1 + __MAX_OUT_SIZE__ + 1 );
							strcpy(response, in_cmd);
							strcpy(response+strlen(in_cmd)+1, input_buf);
							int response_size = strlen(in_cmd)+1+strlen(input_buf)+1;
							int nbytes = write(con_fd, response, response_size);
							if(nbytes != response_size) {
								printf("Error in writing. Exiting...\n");	
							}
							char tmp_buf[__MAX_OUT_SIZE__ + 1];
							int tmp_buf_size;
							tmp_buf_size = read(con_fd, tmp_buf, __MAX_OUT_SIZE__+1);
							tmp_buf[tmp_buf_size] = 0;
							printf("%s\n", tmp_buf);
							strcat(concat_resp, tmp_buf);
							close(con_fd);
							++cur_ip;	
						}
						//concat_resp[concat_offset] = 0;
						input_buf_size = strlen(concat_resp)+1;
						strcpy(input_buf, concat_resp);
					}

					else {
						int con_fd;				
						if(cl[0] != '-') {
							int cl_no = atoi(cl + 1);	
							con_fd = 	clnt_side_setup(config_ips[cl_no-1], __CLIENT_PORT__);
						}
						else {
							con_fd = clnt_side_setup(inet_ntoa(clnt_addr.sin_addr), __CLIENT_PORT__);	
						}
						char response[__MAX_CMD_SIZE__ + 1 + __MAX_OUT_SIZE__ + 1] = {0};
						strcpy(response, in_cmd);
						strcpy(response+strlen(in_cmd)+1, input_buf);
						int response_size = strlen(in_cmd)+1+strlen(input_buf)+1;

						printf("Sending cmd: %s\n", response);
						int nbytes = write(con_fd, response, response_size);
						if(nbytes != response_size) {
							printf("Error in writing. Exiting...\n");	
						}
						input_buf_size = read(con_fd, input_buf, __MAX_OUT_SIZE__+1);
						printf("Pipe out: %s\n", input_buf);
						close(con_fd);
					}
					++cmd_it;
				}

				input_buf[input_buf_size] = 0;
				++input_buf_size;
				printf("Received output: %s\n", input_buf);
				write(clnt_sock, input_buf, input_buf_size);
				printf("reached here\n");
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

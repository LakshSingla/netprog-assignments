#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "constants.h"
#include "tcp_helpers.h"
#include "upload_file.h"

int main() {
	int confd = clnt_side_setup(__NAME_SERVER_IP__, __NAME_SERVER_PORT__);
	while(true) {
		printf(__PROMPT__);	
		size_t cmd_size = __MAX_CMD_SIZE__+1;
		char *cmd_buf = malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act  == -1 || cmd_size_act == 0 || cmd_size_act == 1 && cmd_buf[0] == '\n') continue;
		cmd_buf[cmd_size_act-1] = 0;



		printf("%s\n", cmd_buf);

		char *tok = strtok(cmd_buf, " ");
		
		char *cmd_name = strdup(tok);

		if (strcmp(cmd_name, __UPLOAD_CMD__) == 0) {
			// upload cmd
			tok = strtok(NULL, " ");
			char *filename = tok;
			char cmd_send[__MAX_CMD_SIZE__];
			/*sprintf(cmd_send, "%s#%s#", __UPLOAD_CODE__, filename);*/
			sprintf(cmd_send, "%s%s", __UPLOAD_CODE__, filename);
			write(confd, cmd_send, strlen(cmd_send) * sizeof(char));
			char ack[1];
			read(confd, ack, 1);
			upload_file(filename, confd);
			char resp[__MAX_RESP_SIZE__];
			int nb = read(confd, resp, __MAX_RESP_SIZE__);
			resp[nb] = 0;
			printf("\n%s\n", resp);
		}
		else if (strcmp(cmd_name, __LS_CMD__) == 0) {
			// ls cmd
			char cmd_send[__MAX_CMD_SIZE__];
			sprintf(cmd_send, "%s", __LS_CODE__);
			write(confd, cmd_send, strlen(cmd_send) * sizeof(char));
			char resp[__MAX_PATH_LEN__];
			int nb = read(confd, resp, __MAX_RESP_SIZE__);
			resp[nb] = 0;
			printf("\n%s\n", resp);

		}
		else if (strcmp(cmd_name, __CP_CMD__) == 0) {
			// cp cmd
			
		}
		else if (strcmp(cmd_name, __CD_CMD__) == 0) {
			// cd cmd
			tok = strtok(NULL, " ");
			char *path = tok;
			char cmd_send[__MAX_CMD_SIZE__];
			sprintf(cmd_send, "%s%s", __CD_CODE__, path);
			write(confd, cmd_send, strlen(cmd_send) * sizeof(char));
			char resp[__MAX_PATH_LEN__];
			int nb = read(confd, resp, __MAX_RESP_SIZE__);
			resp[nb] = 0;
			printf("\n%s\n", resp);

		}
		else if (strcmp(cmd_name, __MV_CMD__) == 0) {
			// mv cmd
			
		}
		else if (strcmp(cmd_name, __MKDIR_CMD__) == 0) {
			// mkdir cmd
			tok = strtok(NULL, " ");
			char *path = tok;
			char cmd_send[__MAX_CMD_SIZE__];
			sprintf(cmd_send, "%s%s", __MKDIR_CODE__, path);
			write(confd, cmd_send, strlen(cmd_send) * sizeof(char));
			char resp[__MAX_PATH_LEN__];
			int nb = read(confd, resp, __MAX_RESP_SIZE__);
			resp[nb] = 0;
			printf("\n%s\n", resp);
		
		}
		else if (strcmp(cmd_name, __RM_CMD__) == 0) {
			// rm cmd
			tok = strtok(NULL, " ");
			char *path = tok;
			char cmd_send[__MAX_CMD_SIZE__];
			sprintf(cmd_send, "%s%s", __RM_CODE__, path);
			write(confd, cmd_send, strlen(cmd_send) * sizeof(char));
			char resp[__MAX_PATH_LEN__];
			int nb = read(confd, resp, __MAX_RESP_SIZE__);
			resp[nb] = 0;
			printf("\n%s\n", resp);

		}
		else if (strcmp(cmd_name, __CAT_CMD__) == 0) {
			// cat cmd
			
		}
		else {
			printf("Unknown command\nAllowed commands: %s, %s, %s, %s, %s, %s, %s\n", __UPLOAD_CMD__, __LS_CMD__, __CP_CMD__, __MV_CMD__, __MKDIR_CMD__, __RM_CMD__, __CAT_CMD__);
		}

	}
}

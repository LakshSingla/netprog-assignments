#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"

void read_rem_msg (int fd, char *msg, int size) {
	int nt = read(fd, msg, size);
	msg[nt] = 0;
	if (nt <= 0) {
		printf("Error reading topic from publisher\n");
		printf("Closing Connection...");
		close(fd);
		exit(0);
	}
}

void msg_prefix (char *msg, char *class, char *code, char *content) {
	strcat(msg, class);
	msg[3] = 1;
	msg[4] = 0;
	strcat(msg, code);
	strcat(msg, content);
}

char *send_and_wait (int con_fd, char *msg, int msg_size) {
	// write cmd
	int n = write(con_fd, msg, msg_size);
	if (n != msg_size) {
		printf("Error with write()\n");
	}

	// read response
	char *resp = (char *) malloc(sizeof(char) * __MAX_RESPONSE_SIZE__);
	int nr = read(con_fd, resp, __MAX_RESPONSE_SIZE__);
	if (nr <= 0) {
		printf("Error reading response\n");
	}
	else {
		resp[nr] = 0;
		return resp;
	}
}



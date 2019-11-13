#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"
#include "tcp_helpers.h"

void add_prefix (char *msg, char *class, char *code) {
	strcat(msg, class);
	msg[3] = 1;
	msg[4] = 0;
	strcat(msg, code);
}

void create_topic (char *topic, int con_fd) {
	int msg_size = 4 + 1 + strlen(topic);
	char msg[msg_size];
	memset(msg, 0, msg_size);
	// creating a string of the form-
	// "PUB" + '\0' + code + topic
	add_prefix(msg, __PUB_CLASS__, __PUB_CREATE_CODE__);
	strcat(msg, topic);
	msg[3] = 0;

	// write cmd
	int n = write(con_fd, msg, msg_size);
	if (n != msg_size) {
		printf("Error with write()\n");
	}

	// read response
	char resp[__MAX_RESPONSE_SIZE__];
	int nr = read(con_fd, resp, __MAX_RESPONSE_SIZE__);
	if (nr <= 0) {
		printf("Error reading response\n");
	}
	else {
		printf("\n%s\n", resp);
	}
	close(con_fd);
}

void send_msg (char *topic, char *msg) {

}

void send_file (char *topic, char *filename) {

}

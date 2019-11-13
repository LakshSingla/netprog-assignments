#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"
#include "tcp_helpers.h"

void msg_prefix (char *msg, char *class, char *code, char *content) {
	strcat(msg, class);
	msg[3] = 1;
	msg[4] = 0;
	strcat(msg, code);
	strcat(msg, content);
}

void send_and_wait (int con_fd, char *msg, int msg_size) {
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
}

void create_topic (char *topic, int con_fd) {
	// creating a string of the form-
	// "PUB" + '\0' + code + topic
	int msg_size = 4 + 1 + strlen(topic);
	char msg[msg_size];
	memset(msg, 0, msg_size);
	msg_prefix (msg, __PUB_CLASS__, __PUB_CREATE_CODE__, topic);
	msg[3] = 0;

	send_and_wait (con_fd, msg, msg_size);
	close(con_fd);
}

void send_msg (char *topic, char *msg_send, int con_fd) {
	// creating a string of the form-
	// "PUB" + '\0' + code + topic + message 
	int msg_size = 4 + 1 + strlen(topic) + 1 + strlen(msg_send);
	char msg[msg_size];
	memset(msg, 0, msg_size);
	msg_prefix(msg, __PUB_CLASS__, __PUB_SEND_CODE__, topic);
	strcat(msg, "#");
	strcat(msg, msg_send);
	msg[3] = 0;

	send_and_wait (con_fd, msg, msg_size);
	close(con_fd);
}

void send_file (char *topic, char *filename) {

}

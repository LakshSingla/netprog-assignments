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

void create_topic (char *topic, char *broker_ip, int broker_port) {
	int msg_size = 4 + 1 + strlen(topic);
	char msg[msg_size];
	memset(msg, 0, msg_size);
	// creating a string of the form-
	// "PUB" + '\0' + code + topic
	add_prefix(msg, __PUB_CLASS__, __PUB_CREATE_CODE__);
	strcat(msg, topic);
	msg[3] = 0;

	int con_fd = clnt_side_setup(broker_ip, broker_port);

	int n = write(con_fd, msg, msg_size);
	if (n != msg_size) {
		printf("Error with write()\n");
	}
	close(con_fd);
}

void send_msg (char *topic, char *msg) {

}

void send_file (char *topic, char *filename) {

}

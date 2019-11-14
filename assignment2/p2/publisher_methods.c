#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "constants.h"
#include "publisher_run_cmd.h"
#include "common_utils.h"

void create_topic (char *topic, int con_fd) {
	// creating a string of the form-
	// "PUB" + '\0' + code + topic
	int msg_size = 4 + 1 + strlen(topic);
	char msg[msg_size];
	memset(msg, 0, msg_size);
	msg_prefix (msg, __PUB_CLASS__, __PUB_CREATE_CODE__, topic);
	msg[3] = 0;

	send_and_wait (con_fd, msg, msg_size);
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
}

void send_file (char *topic, char *filename, char *broker_ip, int broker_port) {
	int fd = open(filename, O_RDONLY);

	char buf[__MAX_MSG_SIZE__ + 1];
	int n;
	while ((n = read(fd, buf, __MAX_MSG_SIZE__)) != 0) {
		buf[n] = 0;
		char cmd[__MAX_CMD_SIZE__];
		memset(cmd, 0, __MAX_CMD_SIZE__);
		strcat(cmd, "send ");
		strcat(cmd, topic);
		strcat(cmd, " ");
		strcat(cmd, buf);
		run_cmd(cmd, broker_ip, broker_port);
	}
}

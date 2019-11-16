#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "constants.h"
#include "subscriber.h"
#include "subscriber_run_cmd.h"
#include "common_utils.h"

void subscribe_topic (char *topic) {
	for(int i = 0; i < subscribed_topics_len; ++i) {
		if(strcmp(subscribed_topics[i], topic) == 0) {
			printf("Topic already subscribed\n");
			return;
		}
	}
	subscribed_topics[subscribed_topics_len] = topic;
	subscribed_topics_len++;
} 

void ret_topic (char *topic, int con_fd) {
	int sub_count = subscribed_topics_len;
	int i;
	for (i = 0; i < sub_count; i++) {
		if (strcmp(subscribed_topics[i], topic) == 0) break;
	}

	if (i == sub_count) {
		printf("Not subscribed to the topic!\n");
	}
	else {
		// creating a string of the form-
		// "SUB" + '\0' + code + topic
		char id_n_topic[10 + 1 + __MAX_TOPIC_SIZE__];
		sprintf(id_n_topic, "%d#%s", last_msg_id, topic);
		int msg_size = 4 + 1 + strlen(id_n_topic);
		char msg[msg_size];
		memset(msg, 0, msg_size);
		msg_prefix (msg, __SUB_CLASS__, __SUB_RET_CODE__, id_n_topic);
		msg[3] = 0;

		char *resp = send_and_wait (con_fd, msg, msg_size);

		char *msg_content = strchr(resp, '#');
		if (msg_content != NULL) {
			char *resp_copy = strdup(resp);
			char *msg_id = strtok(resp_copy, "#");
			msg_content += 1;
			last_msg_id = atoi(msg_id);
			printf("Message id: %s\n", msg_id);
			printf("Message content: %s\n", msg_content);
		}
		else {
			printf("\n%s\n", resp);
		}
	}
}

void retall_topic (char *topic, int con_fd) {
	int sub_count = subscribed_topics_len;
	int i;
	for (i = 0; i < sub_count; i++) {
		if (strcmp(subscribed_topics[i], topic) == 0) break;
	}

	if (i == sub_count) {
		printf("Not subscribed to the topic!\n");
	}
	else {
		// creating a string of the form-
		// "SUB" + '\0' + code + topic
		int msg_size = 4 + 1 + strlen(topic);
		char msg[msg_size];
		memset(msg, 0, msg_size);
		msg_prefix (msg, __SUB_CLASS__, __SUB_RETALL_CODE__, topic);
		msg[3] = 0;

		send_and_wait (con_fd, msg, msg_size);
	}
}

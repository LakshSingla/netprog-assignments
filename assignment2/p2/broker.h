#ifndef  _BROKER_H_
#define  _BROKER_H_

#include <arpa/inet.h>
#include "constants.h"

struct broker {
	char ip[INET_ADDRSTRLEN];
	int port;
	char n1_ip[INET_ADDRSTRLEN];
	int n1_port;
	char n2_ip[INET_ADDRSTRLEN];
	int n2_port;
};

extern struct broker BROKERS[3];

struct l_topic {
	int no_messages;
	char topic_name[20];
	char msg_arr[100][512]; //max no messages per topic * topic content
};

struct shared_mem_structure {
	int n;
	struct l_topic lt[100]; //max no of topics
};

struct topic_msg_list {
	/*
	 *char topic[__MAX_TOPIC_SIZE__];
	 *char *msgs[__MAX_MSG_COUNT__];
	 */
	int topic;
};

extern struct topic_msg_list **MAIN_TOPIC_LIST;
extern int *curr_topic_count;
extern int *topic;

#endif

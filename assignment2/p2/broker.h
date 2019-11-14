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
	char topic_name[__MAX_TOPIC_SIZE__];
	char msg_arr[__MAX_TOPIC_COUNT__][__MAX_MSG_SIZE__]; //max no messages per topic * topic content
};

struct shared_mem_structure {
	int n;
	struct l_topic lt[__MAX_TOPIC_COUNT__]; //max no of topics
};


#endif

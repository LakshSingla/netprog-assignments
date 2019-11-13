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

struct topic_msg_list {
	char *topic;
	char *msgs[__MAX_MSG_COUNT__];
};

extern struct topic_msg_list MAIN_TOPIC_LIST[__MAX_TOPIC_COUNT__];
extern int curr_topic_count;

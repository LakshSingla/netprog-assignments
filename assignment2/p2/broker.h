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
	/*
	 *char topic[__MAX_TOPIC_SIZE__];
	 *char *msgs[__MAX_MSG_COUNT__];
	 */
	int topic;
};

extern struct topic_msg_list **MAIN_TOPIC_LIST;
extern int *curr_topic_count;
extern int *topic;

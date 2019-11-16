#ifndef  _BROKER_H_
#define  _BROKER_H_

#include <arpa/inet.h>
#include "constants.h"

struct broker {
	char ip[INET_ADDRSTRLEN];
	int port;
	char left_ip[INET_ADDRSTRLEN];
	int left_port;
	char right_ip[INET_ADDRSTRLEN];
	int right_port;
};

extern struct broker BROKERS[3];

struct msg_struct {
	int id;
	char msg[__MAX_MSG_SIZE__];
};

struct l_topic {
	int no_messages;
	int max_msg_id;
	char topic_name[__MAX_TOPIC_SIZE__];
	struct msg_struct msg_arr[__MAX_MSG_COUNT__]; //max no messages per topic * topic content
	int get_index;
	int set_index;
};

struct shared_mem_structure {
	int n;
	struct l_topic lt[__MAX_TOPIC_COUNT__]; //max no of topics
};

extern struct shared_mem_structure *sh_mem;
extern char *self_ip;
extern int self_port;
extern char *left_ip;
extern int left_port;
extern char *right_ip;
extern int right_port;

#endif

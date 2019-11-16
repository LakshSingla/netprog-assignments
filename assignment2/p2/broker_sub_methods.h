#ifndef _BROKER_PUB_METHODS_H
#define _BROKER_PUB_METHODS_H

#include "broker.h"

	void handle_topic_read (int fd, char *topic, int last_msg_id, struct shared_mem_structure *addr);
#endif

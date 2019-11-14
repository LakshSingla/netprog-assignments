#ifndef _BROKER_PUB_METHODS_H
#define _BROKER_PUB_METHODS_H

#include "broker.h"

	void handle_topic_read (int fd, char *topic, struct shared_mem_structure *addr);
#endif

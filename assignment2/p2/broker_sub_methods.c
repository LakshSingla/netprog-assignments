#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "broker.h"

void handle_topic_read (int fd, char *topic, struct shared_mem_structure *addr) {
	int count = addr->n;
	int i;
	char *reply;
	for(i = 0; i < count; ++i) {
		if(strcmp((addr->lt[i]).topic_name, topic) == 0) {
			// topic present
			if (addr->lt[i].no_messages > 0) {
				write(fd, addr->lt[i].msg_arr[0], strlen(addr->lt[i].msg_arr[0]) * sizeof(char));
				return;
			}
			else {
				// communicate to other brokers to get messages	
				
			}
			break;
		}
	}
	if (i == count) {
		// topic not present

	}

}

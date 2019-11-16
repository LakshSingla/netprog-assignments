#ifndef _BROKER_METHODS_H
#define _BROKER_METHODS_H

int query_neighbour (char *topic, struct shared_mem_structure *addr);
void handle_brok (int fd, char *topic, struct shared_mem_structure *addr);

#endif

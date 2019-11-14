#ifndef _SUBSCRIBER_METHODS_H
#define _SUBSCRIBER_METHODS_H

#include "subscriber.h"

void subscribe_topic (char *topic);
void ret_topic (char *topic, int con_fd);
void retall_topic (char *topic, int con_fd);
#endif

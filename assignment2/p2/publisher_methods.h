#ifndef _PUBLISHER_METHODS_H
#define _PUBLISHER_METHODS_H

void create_topic (char *topic, int con_fd);
void send_msg (char *topic, char *msg, int con_fd);
void send_file (char *topic, char *filename, char *broker_ip, int broker_port);

#endif

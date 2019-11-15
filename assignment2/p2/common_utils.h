#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

void read_rem_msg (int fd, char *msg, int size);
void msg_prefix (char *msg, char *class, char *code, char *content);
char *send_and_wait (int con_fd, char *msg, int msg_size);
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>

#include "constants.h"

#define COURSE_COUNT 5
#define GROUP_PORT 7000

struct course {
	char name[__MAX_CNAME_SIZE__];
	char group_ip[INET_ADDRSTRLEN];
	int send_fd;
	struct sockaddr_in send_addr;
	int recv_fd;
	struct sockaddr_in recv_addr;
	bool is_joined;
};

// predefined course list
struct course course_list[COURSE_COUNT] = {
	{"course1", "239.0.0.5", -1, -1, false},
	{"course2", "239.0.0.6", -1, -1, false},
	{"course3", "239.0.0.7", -1, -1, false},
	{"course4", "239.0.0.8", -1, -1, false},
	{"course5", "239.0.0.9", -1, -1, false}
};

int maxfd = 0;
fd_set rset, allrset;

int create_sock_fds () {
	// recv socket
	struct sockaddr_in addr2;
	memset(&addr2, 0, sizeof(addr2));
	addr2.sin_family = AF_INET;
	addr2.sin_addr.s_addr = htonl(INADDR_ANY);
	addr2.sin_port = htons(GROUP_PORT);

	int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock2 < 0) {
		perror("Error with socket() in sock2");
		exit(0);
	}
	FD_SET(sock2, &allrset);

	if (bind(sock2, (struct sockaddr *) &addr2, sizeof(addr2)) < 0) {
		perror("Error with bind() in sock2");
		exit(0);
	}

	for (int i = 0; i < COURSE_COUNT; i++) {
		// send socket
		struct sockaddr_in addr1 = addr2;
		addr1.sin_addr.s_addr = inet_addr(course_list[i].group_ip);

		int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock1 < 0) {
			perror("Error with socket() in sock1");
			exit(0);
		}

		course_list[i].send_fd = sock1;
		course_list[i].send_addr = addr1;
		
		course_list[i].recv_fd = sock2;
		course_list[i].recv_addr = addr2;
	}
}

void list_joined_groups () {
	printf("List of joined groups:\n");
	for (int i = 0; i < COURSE_COUNT; i++) {
		if (course_list[i].is_joined == true) {
			printf("%s\n", course_list[i].name);
		}
	}
}

void join_group (char *group_name) {
	int i;
	for (i = 0; i < COURSE_COUNT; i++) {
		if (strcmp(course_list[i].name, group_name) == 0) {
			if (course_list[i].is_joined == true) {
				printf("Group : %s already joined\n", group_name);
			}
			else {
				struct ip_mreq mreq;
				mreq.imr_multiaddr.s_addr = inet_addr(course_list[i].group_ip);
				mreq.imr_interface.s_addr = htonl(INADDR_ANY);

				if (setsockopt(course_list[i].recv_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
					perror("Error joining the group due to setsockopt()");
					exit(0);
				}
				course_list[i].is_joined = true;
				if (course_list[i].recv_fd > maxfd) maxfd = course_list[i].recv_fd;

				printf("Group : %s joined successfully\n", group_name);
			}
			break;
		}
	}
	if (i == COURSE_COUNT) {
		printf("Group : %s not found\n", group_name);
	}
}

void leave_group (char *group_name) {
	int i;
	for (i = 0; i < COURSE_COUNT; i++) {
		if (strcmp(course_list[i].name, group_name) == 0) {
			if (course_list[i].is_joined == false) {
				printf("Group : %s already not joined\n", group_name);
			}
			else {
				struct ip_mreq mreq;
				mreq.imr_multiaddr.s_addr = inet_addr(course_list[i].group_ip);
				mreq.imr_interface.s_addr = htonl(INADDR_ANY);

				if (setsockopt(course_list[i].recv_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
					perror("Error leaving the group due to setsockopt()");
					exit(0);
				}
				course_list[i].is_joined = false;
				/*FD_CLR(course_list[i].recv_fd, &allrset);*/

				printf("Group : %s left successfully\n", group_name);
			}
			break;
		}
	}
	if (i == COURSE_COUNT) {
		printf("Group : %s not found\n", group_name);
	}
}

void notify_group (char *group_name) {
	int sock;
	struct sockaddr_in addr;

	int i;
	for (i = 0; i < COURSE_COUNT; i++) {
		if (strcmp(course_list[i].name, group_name) == 0) {
			if (course_list[i].is_joined == false) {
				printf("Group : %s not joined\n", group_name);
				return;
			}
			else {
				sock = course_list[i].send_fd;
				addr = course_list[i].send_addr;
			}
			break;
		}
	}
	if (i == COURSE_COUNT) {
		printf("Group : %s not found\n", group_name);
		return;
	}

	printf("Notifying group : %s\n", group_name);
	printf("You'll be informed when the group is notified\n");
	if (fork() == 0) {
		char msg[__MAX_MSG_SIZE__];
		sprintf(msg, "Surprise test in %s!", group_name);
		if (sendto(sock, msg, sizeof(msg), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("Error notifying group due to sendto()");
		}
		else {
			char notified[__MAX_MSG_SIZE__];
			sprintf(notified, "\nGroup : %s has been notified\n%s", group_name, __PROMPT__);
			/*write(0, __PROMPT__, strlen(__PROMPT__) * sizeof(char));	*/
			write(0, notified, strlen(notified) * sizeof(char));	
		}

		exit(0);
	}
}

int main (int argc, char *argv[]) {
	FD_ZERO(&allrset);
	FD_SET(0, &allrset);

	create_sock_fds();
	
	while (true) {
		write(0, __PROMPT__, strlen(__PROMPT__) * sizeof(char));	
		rset = allrset;

		int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(0, &rset)) {
			size_t cmd_size = __MAX_CMD_SIZE__+1;
			char *cmd_buf = malloc(sizeof(char) * cmd_size);
			ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
			if(cmd_size_act  == -1 || cmd_size_act == 0 || cmd_size_act == 1 && cmd_buf[0] == '\n') continue;
			cmd_buf[cmd_size_act-1] = 0;

			char *cmd_copy = strdup(cmd_buf);
			char *tok = strtok(cmd_buf, " ");
			
			if (tok == NULL) {
				printf("Unknown command\n");
				continue;
			}
			char *cmd_name = strdup(tok);

			if (strcmp(cmd_name, __GRPLST_CMD__) == 0) {
				list_joined_groups();
			}
			else {
				char *tmp_ptr = strchr(cmd_copy, ' ');
				if (tmp_ptr == NULL) {
					printf("Incorrect format for cmd\n");
					continue;
				}

				char *group_name = tmp_ptr + 1;

				if (group_name == NULL) {
					printf("Group name required for the command\n");
					continue;
				}

				if (strcmp(cmd_name, __JOIN_CMD__) == 0) {
					join_group(group_name);
				}
				else if (strcmp(cmd_name, __NOTIFY_CMD__) == 0) {
					notify_group(group_name);
				}
				else if (strcmp(cmd_name, __LEAVE_CMD__) == 0) {
					leave_group(group_name);
				}
				else {
					printf("Unknown command\n");
					continue;
				}
			}
			
			nready--;
			if (nready == 0) continue;
			free(cmd_copy);
		}

		for (int i = 0; i < COURSE_COUNT; i++) {
			if (course_list[i].is_joined == true) {
				int sock = course_list[i].recv_fd;
				struct sockaddr_in addr = course_list[i].recv_addr;
				int addr_len = sizeof(addr);
				if (FD_ISSET(course_list[i].recv_fd, &rset)) {
					char msg[__MAX_MSG_SIZE__];
					if (recvfrom(sock, msg, sizeof(msg), 0, (struct sockaddr *) &addr, &addr_len) < 0) {
						perror("Error with receiving notification due to recvfrom()");
					}
					else {
						printf("\nNotification:\n%s\n", msg);
					}
					nready--;
					if (nready == 0) break;
				}
			}
		}
	}
}

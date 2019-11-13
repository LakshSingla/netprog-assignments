#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "broker.h"
#include "constants.h"

int *curr_topic_count;
struct topic_msg_list **MAIN_TOPIC_LIST;
int *topic;

void handle_topic_create (int fd, char *topic) {
	int i = 0;
	char *reply;

	printf("topic count = %d\n", *curr_topic_count);
	while (i < *curr_topic_count) {
		printf("asdfasdf\n");
		/*printf("hererere: %s\n", MAIN_TOPIC_LIST[i]->topic);*/
		printf("hererere %d: %d\n", i, topic[i]);
		/*
		 *if (strcmp(MAIN_TOPIC_LIST[i]->topic, topic) == 0) {
		 *        reply = "Topic already exists!";
		 *        break;
		 *}
		 */
		i++;
	}

	if (i == *curr_topic_count) {
		if (*curr_topic_count == __MAX_TOPIC_COUNT__) {
			reply = "Max topic count reached!";
		}
		else {
/*
 *                        int shmkey = ftok("./broker.c", getpid());
 *                        printf("shmkey = %d\n", shmkey);
 *                        int shmid = shmget(shmkey, sizeof(struct topic_msg_list), 
 *                                        IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
 *                        if (shmid == -1) {
 *                                perror("Error with shmget()");
 *                                exit(0);
 *                        }
 *
 *                        MAIN_TOPIC_LIST[*curr_topic_count] = shmat(shmid, NULL, 0);
 *                        if (MAIN_TOPIC_LIST[*curr_topic_count] == (void *) -1) {
 *                                perror("Error with shmat()");
 *                                exit(0);
 *                        }
 */
			/*MAIN_TOPIC_LIST[*curr_topic_count] = (struct topic_msg_list *) malloc(sizeof(struct topic_msg_list));*/
			/*MAIN_TOPIC_LIST[*curr_topic_count]->topic = (char *) malloc((strlen(topic) + 1) * sizeof(char));*/
			/*strcpy(MAIN_TOPIC_LIST[*curr_topic_count]->topic, topic);*/
			printf("initial topic = %d\n", topic[*curr_topic_count]);
			/*topic[*curr_topic_count] = 123;*/
			*(topic + *curr_topic_count) = 123;
			*curr_topic_count = *curr_topic_count + 1;
			reply = "Topic successfully created!";
		}
	}

	write(fd, reply, sizeof(char) * (strlen(reply) + 1));
}

void handle_msg_recv (int clnt_sock, char *topic, char *msg) {
	printf("topic: %s\nmsg: %s\n", topic, msg);

}

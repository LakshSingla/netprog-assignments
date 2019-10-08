#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "constants.h"

char *get_canonical_path(char *exec_path){
	char *canonical_path = NULL;
	if(exec_path[0] == '.' || exec_path[0] == '/') {
		canonical_path = strdup(exec_path);
	}
	else {
		char *env_path = strdup(getenv(__PATH_ENV_VAR__));
		//printf("ENV: %s\n", env_path);
		char *token = strtok(env_path, ":");
		bool found = false;
		while(token != NULL) {
			//printf("TOK: %s\n", token);
			char *maybe_path = strdup(token);
			maybe_path = realloc(maybe_path, strlen(maybe_path) + __MAX_CMD_SIZE__ + 10); //10 -> arbitrary padding
			strcat(maybe_path, "/");
			strcat(maybe_path, exec_path);
			//printf("MB_PATH: %s\n", maybe_path);

			struct stat sbuf;

			if(stat(maybe_path, &sbuf) == -1) {
				if(errno == ENOENT) {
					//return NULL;
				}
				else {
					printf(__EMSG_UNHANDLED__);
					exit(-1);	
				}
			}

			else {
				if(S_ISREG(sbuf.st_mode) && (sbuf.st_mode && (S_IXUSR | S_IXGRP | S_IXOTH))) {
					found = true;
				}
			}
			if(found) {
				canonical_path = strdup(maybe_path);	
				free(maybe_path);
				break;
			}
			free(maybe_path);
			token = strtok(NULL, ":");
		}

		free(env_path);
	}

	return canonical_path;
}


void FREE_CANONICAL_PATH(char *path) {
	free(path);
}

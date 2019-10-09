#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "get_canonical_path.h"

/*
	Executed a single command in which |,#,S,> have been removed
*/
void exec_cmd (char *cmd) {
	char *tmp_cmd = strdup(cmd);
	char *tmp_cmd2 = strdup(cmd);
	int token_cnt = 0;
	if (tmp_cmd != NULL && strcmp(tmp_cmd, "")) {

		// counting the number of args
		char *token = strtok(tmp_cmd, " ");
		token_cnt = 0;
		while (token != NULL) {
			token_cnt++;
			token = strtok(NULL, " ");
		}

		// storing the args in a vector
		char *cmd_args[token_cnt + 1];
		char *token2 = strtok(tmp_cmd2, " ");
		int i = 0;
		while (token2 != NULL) {
			cmd_args[i] = token2;
			i++;
			token2 = strtok(NULL, " ");
		}
		cmd_args[i] = NULL;

//		for (int j = 0; j < token_cnt; j++) printf("aa: %s\n", cmd_args[j]);

		char *canonical_path = get_canonical_path(cmd_args[0]);

		if (execv(canonical_path, cmd_args) == -1) {
			printf("Error executing command: %d\n", errno);
		}

		FREE_CANONICAL_PATH(canonical_path);
	}

	free(tmp_cmd);
	free(tmp_cmd2);
}

#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "get_canonical_path.h"
#include "parse_cmd.h"
#include "constants.h"

enum pipe_medium {PIPE, MSGQ, SHM};
typedef enum pipe_medium PIPE_MEDIUM;

struct pipe_info {
	PIPE_MEDIUM type;
	int read_id[2];
	int write_id[2];
};
typedef struct pipe_info PIPE_INFO;

PIPE_INFO* CREATE_PIPE_TYPE (PIPE_MEDIUM type, int read_id[], int write_id[]) {
	PIPE_INFO *pi = (PIPE_INFO *) malloc(sizeof(PIPE_INFO));
	pi->type = type;
	pi->read_id[0] = read_id[0];
	pi->read_id[1] = read_id[1];
	pi->write_id[0] = write_id[0];
	pi->write_id[1] = write_id[1];

	return pi;
}

/*
	Executed a single command in which |,#,S,> have been removed
*/
void exec_single_cmd (char *cmd) {
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


void READ_EXEC_WRITE (PIPE_INFO *from, char *cmd, PIPE_INFO *to, int psync[]) {
	int inp_size = 0, out_size = 0;
	char inp[__MAX_OUT_SIZE__], out[__MAX_OUT_SIZE__];

	if (from != NULL) {
		close(0);
		dup(from->read_id[0]);
	}
	printf("test: %d\n", to->write_id[1]);

	close(1);
	dup(to->write_id[1]);

	exec_single_cmd(cmd);
}

/*
	Execute a complete command
*/
void exec_cmd (char *cmd) {
	PARSED_CMD *parsed = parse_cmd(cmd);

	//sync pipe
	int psync[2];
	pipe(psync);

	// creating pipe
	int p1[2];
	pipe(p1);
	int p2[2];
	pipe(p2);

	int p_reads[2] = {p1[0], p2[0]};
	int p_writes[2] = {p1[1], p2[1]};

	PIPE_INFO *pipe_pipe = CREATE_PIPE_TYPE(PIPE, p_reads, p_writes);

	PIPE_INFO *read_end = NULL, *write_end = NULL;
	int prev = 0;

	// execution
	int dim = 0;
	while (dim < parsed->dim) {
		int i = 0;
		while (parsed->cmd_opt_list[dim][i] != NULL) {
//			printf("a: %s\n", parsed->cmd_opt_list[dim][i]);

			char *single_cmd = parsed->cmd_opt_list[dim][i];
			i++;

			if (i == 1) {
				read_end = NULL;
			}
			else read_end = write_end;

			if (parsed->cmd_opt_list[dim][i] != NULL) {
				char *single_opt = parsed->cmd_opt_list[dim][i];
				i++;

				if (strcmp(single_opt, "|") == 0) {
					write_end = pipe_pipe;
				}
			}
			else {
				write_end = pipe_pipe; // the final output written in pipe
			}

			pid_t pid = fork();
			if (pid == 0) {
				printf("asdf\n");
				READ_EXEC_WRITE(read_end, single_cmd, write_end, psync);
			}

			printf("test: %d\n", write_end->write_id[1]);
			char out[__MAX_OUT_SIZE__];
			int numread = read(write_end->read_id[1], out, __MAX_OUT_SIZE__);
			printf("here: %d\n", numread);
			out[numread] = 0;

			write(write_end->write_id[0], out, numread+1);
/*			char out2[__MAX_OUT_SIZE__];
			int numread2 = read(write_end->read_id[0], out2, __MAX_OUT_SIZE__);
			printf("here2: %d\n", numread);*/
		}

		dim++;
	}

	char out[__MAX_OUT_SIZE__];
	int numread = read(write_end->read_id[0], out, __MAX_OUT_SIZE__);
	out[numread] = 0;
	printf("out: %s\n", out);

	free(pipe_pipe);
}

int main () {
	exec_cmd("ls|wc");
}

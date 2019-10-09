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
	int read_id;
	int write_id;
};
typedef struct pipe_info PIPE_INFO;

PIPE_INFO* CREATE_PIPE_TYPE (PIPE_MEDIUM type, int read_id, int write_id) {
	PIPE_INFO *pi = (PIPE_INFO *) malloc(sizeof(PIPE_INFO));
	pi->type = type;
	pi->read_id = read_id;
	pi->write_id = write_id;

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


void READ_EXEC_WRITE (PIPE_INFO *from, char *cmd, PIPE_INFO *to) {
	int inp_size = 0, out_size = 0;
	char inp[__MAX_OUT_SIZE__], out[__MAX_OUT_SIZE__];

//	if (from != NULL) fprintf(stderr, "fds1: %d %d\n", from->read_id, from->write_id);
//	fprintf(stderr, "fds2: %d %d\n", to->read_id, to->write_id);

	if (from != NULL) {
		close(from->write_id);
		close(0);
		dup(from->read_id);
	}

	close(1);
	dup(to->write_id);

	exec_single_cmd(cmd);
}

/*
	Execute a complete command
*/
void exec_cmd (char *cmd) {
	PARSED_CMD *parsed = parse_cmd(cmd);

	// creating pipe
//	int p[2];
//	pipe(p);

//	PIPE_INFO *pipe_pipe = CREATE_PIPE_TYPE(PIPE, p[0], p[1]);
	PIPE_INFO *pipe_pipe;
	PIPE_INFO *shm_pipe;
	PIPE_INFO *msgq_pipe;

	// creating output pipe
	int p_out[2];
	pipe(p_out);

	PIPE_INFO *pipe_pipe_out = CREATE_PIPE_TYPE(PIPE, p_out[0], p_out[1]);

	PIPE_INFO *read_end = NULL, *write_end = NULL;
	int prev = 0;

	// execution
	int dim = 0;
	while (dim < parsed->dim) {
		int i = 0;
		while (parsed->cmd_opt_list[dim][i] != NULL) {

			char *single_cmd = parsed->cmd_opt_list[dim][i];
			i++;

			if (i == 1) {
				read_end = NULL;
			}
			else {
				read_end = write_end;
				close(read_end->write_id);
			}

			if (parsed->cmd_opt_list[dim][i] != NULL) {
				char *single_opt = parsed->cmd_opt_list[dim][i];
				i++;

				if (strcmp(single_opt, "|") == 0) {
					int p[2];
					pipe(p);

					pipe_pipe = CREATE_PIPE_TYPE(PIPE, p[0], p[1]);
					write_end = pipe_pipe;
				}
				else if (strcmp(single_opt, "#") == 0) {
					int p[2];
					pipe(p);

					pipe_pipe = CREATE_PIPE_TYPE(PIPE, p[0], p[1]);
					write_end = pipe_pipe;
				}
			}
			else {
				write_end = pipe_pipe_out; // the final output written in pipe
			}

			pid_t pid = fork();
			if (pid == 0) {
				READ_EXEC_WRITE(read_end, single_cmd, write_end);
			}
			else {
				int status;
				waitpid(pid, &status, 0);
				while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status)) {
					waitpid(pid, &status, 0);
				}
			}
		}

		dim++;
	}

	char out[__MAX_OUT_SIZE__];
	int numread = read(write_end->read_id, out, __MAX_OUT_SIZE__);
	out[numread] = 0;
	printf("out:\n%s\n", out);

	free(pipe_pipe);
}

int main () {
	exec_cmd("cat README.md|wc");
}

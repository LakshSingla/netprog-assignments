#include	<stdio.h>
#include	<unistd.h>
#include	<wait.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<errno.h>

#include "get_canonical_path.h"
#include "parse_cmd.h"
#include "daemonize.h"
#include "constants.h"
#include "job_control.h"

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
int exec_single_cmd (char *cmd) {
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

		char *canonical_path = get_canonical_path(cmd_args[0]);
		if (canonical_path == NULL) {
			fprintf(stderr, "%s: Command not found\n", cmd_args[0]);
			return 0;
		}
//		fprintf(stderr, "here: %s\n", canonical_path);

		if (execv(canonical_path, cmd_args) == -1) {
			fprintf(stderr, "Error executing command: %d\n", errno);
		}

		FREE_CANONICAL_PATH(canonical_path);
		free(tmp_cmd);
		free(tmp_cmd2);
		free(cmd);
	}

}

char * handle_redirection (PIPE_INFO *from, char *cmd, PIPE_INFO *to) {
	char *cmd_copy = strdup(cmd);

	char *token = strtok(cmd_copy, "><");

	if (strcmp(token, cmd) != 0) {
//		printf("asdf: %c\n", *(cmd + strlen(token)));
		char *file;
		if (*(cmd + strlen(token) + 1) == '>') {
			file = cmd + strlen(token) + 2;

			int fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0664);

			if (fd < 0) fprintf(stderr, "Cannot open file %s\n", file);
			else {
				if (to != NULL) {
					close(to->write_id);
				}
				else {
					close(1);
				}
				dup(fd);
			}
			return token;
		}
		else if (*(cmd + strlen(token)) == '>') {
			file = cmd + strlen(token) + 1;
			int fd = open(file, O_CREAT | O_WRONLY, 0664);

			if (fd < 0) fprintf(stderr, "Cannot open file %s\n", file);
			else {
				if (to != NULL) {
					close(to->write_id);
				}
				else {
					close(1);
				}
				dup(fd);
			}
			return token;
		}
		else if (*(cmd + strlen(token)) == '<') {
			file = cmd + strlen(token) + 1;

			int fd = open(file, O_CREAT | O_RDONLY, 0664);

			if (fd < 0) fprintf(stderr, "Cannot open file %s\n", file);
			else {
				if (from != NULL) {
					close(from->read_id);
				}
				else {
					close(0);
				}
				dup(fd);
			}
			return token;
		}

	}
	return cmd;
}

int READ_EXEC_WRITE (PIPE_INFO *from, char *cmd, PIPE_INFO *to) {
	fprintf(stderr, "**%s** command:\n", cmd);

	int inp_size = 0, out_size = 0;
	char inp[__MAX_OUT_SIZE__], out[__MAX_OUT_SIZE__];

	int curr_pid = getpid();
	fprintf(stderr, "\tProcess Id: %d\n", getpid());
	fprintf(stderr, "\tProcess Group Id: %d\n\n", getpgid(curr_pid));

	fprintf(stderr, "\tReading from fd ");
	if (from != NULL) {
		fprintf(stderr, "%d\n", from->read_id);
		close(from->write_id);
		close(0);
		dup(from->read_id);
	}
	else fprintf(stderr, "(null)\n");

	fprintf(stderr, "\tWriting to fd ");
	if (to != NULL) {
		fprintf(stderr, "%d\n", to->write_id);
		close(1);
		dup(to->write_id);
	}
	else {
		fprintf(stderr, "1\n");
		fprintf(stderr, "\n================================================================\n\n");
	}

	char *cmd_broken = handle_redirection(from, cmd, to);

	return exec_single_cmd(cmd_broken);
}

/*
	Execute a complete command
*/
int exec_cmd (char *cmd) {
	char **args = check_cmd(cmd, "daemonize");
	if (args[0] != NULL) {

		printf("Creating new daemon for the program: %s\n", args[1]);
//		if (fork() == 0) {
			make_daemon(args[1]);
//		}
		return 1;
	}

	FREE_CHECK_CMD_ARGS(args);

	args = check_cmd(cmd, __CMD_BG__);
	if (args[0] != NULL) {
		bg(atoi(args[1]));
		return 1;
	}

	FREE_CHECK_CMD_ARGS(args);

	args = check_cmd(cmd, __CMD_FG__);
	if (args[0] != NULL) {
		fg(atoi(args[1]));
		return 1;
	}

	FREE_CHECK_CMD_ARGS(args);

	args = check_cmd(cmd, __CMD_JOBS__);
	if (args[0] != NULL) {
		print_details();
		return 1;
	}

	FREE_CHECK_CMD_ARGS(args);


	PARSED_CMD *parsed = parse_cmd(cmd);

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
				write_end = NULL;
			}

			int p_sync_fg[2];
			pipe(p_sync_fg);

			pid_t child_executer = fork();

			if(child_executer < 0) {
				printf("Error spawning child. Exiting...\n");
				exit(0);
			}
			else if (child_executer == 0) {
				close(p_sync_fg[1]);

            	char buf_sync_fg[3];
            	int n = read(p_sync_fg[0], buf_sync_fg, 3);

            	//setpgid(0, child_executer);

				if (READ_EXEC_WRITE(read_end, single_cmd, write_end) == 0) {
					exit(1);
				};
			}
			else {
				close(p_sync_fg[0]);

            	/*if(setpgid(child_executer, child_executer) == -1) {
                	printf("Unable to create a new process group. Exiting command...\n");
                	exit(0);
            	}

            	signal(SIGTTOU, SIG_IGN);

            	if(tcsetpgrp(STDIN_FILENO, child_executer) == -1) {
                	printf("Unable to set process group as foreground. Exiting command...\n");
                	exit(0);
            	}*/

            	write(p_sync_fg[1], "TTT", 3);

	        	int child_executer_status;
            	do {
                	waitpid(child_executer, &child_executer_status, WUNTRACED);
            	} while(
                    	!WIFSIGNALED(child_executer_status)			&&
                    	!WIFEXITED(child_executer_status)			&&
                    	!WIFSTOPPED(child_executer_status)
               	);

				/*tcsetpgrp(0, getpid());
				signal(SIGTTOU, SIG_DFL);*/

			}
		}

		dim++;
	}
	exit(1);
}

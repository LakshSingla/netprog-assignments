#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#include "constants.h"
#include "get_canonical_path.h"
#include "exec_cmd.h"

int main() {
	while(true) {
		printf(__PROMPT__);

		size_t cmd_size = __MAX_CMD_SIZE__ + 1;
		char *cmd_buf =	malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act == -1 || cmd_size_act == 0) { /* error */ }
		cmd_buf[cmd_size_act-1] = 0;

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

			exec_cmd(cmd_buf);

			}
			else {
				close(p_sync_fg[0]);
				if(setpgid(child_executer, child_executer) == -1) {
					printf("Unable to create a new process group. Exiting command...\n");
					exit(0);
				}
				signal(SIGTTOU, SIG_IGN);
				if(tcsetpgrp(STDIN_FILENO, child_executer) == -1) {
					printf("Unable to set process group as foreground. Exiting command...\n");
					exit(0);
       	}

       	write(p_sync_fg[1], "TTT", 3);

	     int child_executer_status;
			 do {
					waitpid(child_executer, &child_executer_status, WUNTRACED);
       	} while(
               	!WIFSIGNALED(child_executer_status)			&&
               	!WIFEXITED(child_executer_status)			&&
               	!WIFSTOPPED(child_executer_status)
          	);
				tcsetpgrp(0, getpid());
				signal(SIGTTOU, SIG_DFL);
			}
		//exec_cmd(cmd_buf);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#include "constants.h"
#include "get_canonical_path.h"

int main() {
	while(true) {
		printf(__PROMPT__);	
		size_t cmd_size = __MAX_CMD_SIZE__ + 1;
		char *cmd_buf =	malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act == -1 || cmd_size_act == 0) { /* error */ }
		cmd_buf[cmd_size_act-1] = 0;
		

		char *canonical_path = get_canonical_path(cmd_buf);

		if(canonical_path == NULL) {
			printf("%s: Command not found\n", cmd_buf);	
			FREE_CANONICAL_PATH(canonical_path);
			continue;
		}

		pid_t child_executer = fork();
		int child_executer_status;

		int p_sync_fg[2];
		pipe(p_sync_fg);

		if(child_executer < 0) {
			//error	
		}

		else if(child_executer > 0) {
			close(p_sync_fg[0]);
			//printf("waiting for child\n");
			//printf("Parent pgid: %d\n", getpgid(0));

			if(setpgid(child_executer, child_executer) == -1) {
				printf("Unable to create a new process group. Exiting command...\n");
				exit(0);
			}

			printf("Current terminal controlling process: %d\n", tcgetpgrp(STDIN_FILENO));
			signal(SIGTTOU, SIG_IGN);
			if(tcsetpgrp(STDIN_FILENO, child_executer) == -1) {
				printf("Unable to set process group as foreground. Exiting command...\n");
				perror("se: ");
				exit(0);
			}
			//write(p_sync_fg[1], "TTT", 3);
			do{
				waitpid(child_executer, &child_executer_status, WUNTRACED);
			}while(
					!WIFSIGNALED(child_executer_status) 		&&
					!WIFEXITED(child_executer_status)				&&
					!WIFSTOPPED(child_executer_status) 		
				);
			printf("here\n");
			//setpgid(0, child_executer);
			signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(STDIN_FILENO, getpid());
			signal(SIGTTOU, SIG_DFL);
			printf("waited for child\n");
			//close(p_sync_fg[1]);
		}

		else {
			close(p_sync_fg[1]);
			char buf_sync_fg[3];
			int n = read(p_sync_fg[0], buf_sync_fg, 3);
			printf("prev buff");
			write(2, buf_sync_fg, 3);
			printf("\nnbytes: %d\n", n);
			if(n < 0) {
				perror("read: ");	
			}
			//sleep(1);
			printf("here1\n");
			printf("child pgid1: %d\n", getpgid(0));
			int stdin_pgid = tcgetpgrp(STDIN_FILENO);
			printf("Current terminal controlling process: %d\n", stdin_pgid);

			setpgid(0, child_executer);
			/*if(setpgid(0, 0) == -1) {
				printf("Unable to create a new process group. Exiting command...\n");
				exit(0);
			}
			printf("child pgid2: %d\n", getpgid(0));
			signal(SIGTTOU, SIG_IGN);
			if(tcsetpgrp(STDIN_FILENO, stdin_pgid) == -1) {
				printf("Unable to set process group as foreground. Exiting command...\n");
				perror("se: ");
				exit(0);
			}
			signal(SIGTTOU, SIG_DFL);*/
			printf("asdf Current terminal controlling process: %d\n", tcgetpgrp(STDIN_FILENO));
			//tcsetpgrp(STDIN_FILENO, stdin_pgid);
			printf("execing: %s\n", canonical_path);
			execl(canonical_path, canonical_path, (char*)0);
		}


		printf("%s\n", canonical_path);
		

		FREE_CANONICAL_PATH(canonical_path);
	}
}

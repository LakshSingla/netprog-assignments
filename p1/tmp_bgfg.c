#include<stdio.h>
#include<signal.h>
#include<wait.h>
#include<unistd.h>
#include<errno.h>

int main () {
	printf("Beginning...\n");
	pid_t pid;
	if ((pid = fork()) == 0) {
		printf("Enter child\n");


		sleep(2);

		printf("Controlling terminal (in child): %d\n", tcgetpgrp(STDIN_FILENO));
		printf("cat working:\n");
		execlp("cat", "cat", (char *) 0);
	}
	else {
		printf("Parent again\n");

		if (setpgid(pid, pid) == -1) {
			printf("setpgid err\n");
		};

		printf("Controlling terminal: %d\n", tcgetpgrp(STDIN_FILENO));
		signal(SIGTTOU, SIG_IGN);
		if (tcsetpgrp(STDIN_FILENO, pid) < 0) {
			printf("tcsetpgrp err: %d\n", errno);
		}
		printf("Controlling terminal: %d\n", tcgetpgrp(STDIN_FILENO));
		int status;
		waitpid(pid, &status, 0);
		while(!WIFSIGNALED(status)) {
			waitpid(pid, &status, 0);
		}
		printf("parent\n");
	}
}

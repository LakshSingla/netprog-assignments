#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.h"
#include "exec_cmd.h"

// Call from the child process, child process will exit
// Donot call direclty from the shell command
void make_daemon(char *cmd) {
	// Unset creation mask, so that daemon can freely do what it want with file
	umask(0);
	pid_t chpid = fork();
	if(chpid < 0) {
		printf("Forking error. Exiting...\n");	
		exit(-1);
	}
	else if(chpid > 0){
		exit(0);			
	}


	// DONOT PRINT ANYTHING AFTER THIS
	// Create a new session
	setsid();
	if(signal(SIGHUP, SIG_IGN) == SIG_ERR) {
			// Error handled	
			exit(-1);
	}

	// Create another child which will not be a session leader
	if((chpid = fork()) < 0) exit(-1);
	else if(chpid == 0) exit(0);

	int max_possible_fd;
	
	max_possible_fd = sysconf(_SC_OPEN_MAX);

	if(max_possible_fd == -1) max_possible_fd = __MAX_FD_LIMIT__;

	for(int i = 0; i < max_possible_fd; ++i) close(i);

	int fd0 = open("/dev/null", O_RDWR);
	int fd1 = dup(0);
	int fd2 = dup(0);


	if(fd0 != 0 || fd1 != 1 || fd2 != 2) exit(-1);

	exec_cmd(cmd);
}

/*int main() {
	make_daemon("./b.out");
}*/

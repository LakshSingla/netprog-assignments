#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "constants.h"
#include "tcp_helpers.h"

struct cmd_out {
	char *out;
	int nbytes;
};

void run_shell (int serv_port) {
	int con_fd = clnt_side_setup(NULL, serv_port);
	while (1) {
		printf("Enter shell commands\n");
		printf("> ");

		char *cmd = (char *) malloc(sizeof(char) * __MAX_CMD_SIZE__);
		size_t size = __MAX_CMD_SIZE__;
		size_t numread_shell = getline(&cmd, &size, stdin);
		cmd[numread_shell - 1] = '\0'; // don't need \n at the end

		if (strcmp(cmd, __EXIT_CMD__) == 0) break;

//		printf("sending to server: %s\n", cmd);

		// send input command to the server
		write(con_fd, cmd, numread_shell - 1);
		// read output of command from the server
		char buf[__MAX_OUT_SIZE__ + 1];
		printf("xx\n");
		size_t numread_server = read(con_fd, buf, __MAX_OUT_SIZE__);
		printf("xxx\n");

		if (numread_server < 0) {
			printf("Error reading from server\n");
			break;
		}

		buf[numread_server] = '\0';

		printf("result: %s\n", buf);
	}
	close(con_fd);
}

struct cmd_out exec_cmd (char *cmd_inp, int cmd_inp_size) {
//	printf("cmd from server here: %s\n", cmd_inp);
	char *cmd, *inp;
	cmd = cmd_inp;
	inp = cmd_inp;

/*	int i = 0;
	while (cmd_inp[i] != 0) {
		inp++;
		i++;
	}
	inp++;*/
	inp = cmd_inp + strlen(cmd_inp) + 1;
	int i = strlen(cmd_inp);

	struct cmd_out output;

	if (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
		// cd command
		char *path = cmd + 3;

//		printf("cmd -> %s\n", path);
		if (chdir(path) < 0) {
			printf("Error changing directory: %d\n", errno);
		}
	}
	else {
		int p[2];
		pipe(p);

		write(p[1], inp, cmd_inp_size - i - 1);

//		printf("inp: %s\ncmd: %s\n", inp, cmd);

		close(p[1]);
//		close(0);
//		dup(p[0]);
		dup2(p[0], 0);

		FILE *fd = popen(cmd, "r");

		char x[__MAX_OUT_SIZE__];

		output.nbytes = read(fileno(fd), x, __MAX_OUT_SIZE__);
		if (output.nbytes < 0) {
			printf("Error reading output from pipe...\n");
			exit(0);
		}
		x[output.nbytes] = 0;
//		printf("STDIN: %s\n", x);
		output.out = x;
		close(p[0]);
	}
	return output;
}

int main () {
	pid_t pid;

	if ((pid = fork()) == 0) {
		// tcp communication

		int clnt_sock, serv_sock = serv_side_setup(__CLIENT_PORT__); // here client acts as the server

		for (;;) {
			struct sockaddr_in clnt_addr;
        	int clnt_len = sizeof(clnt_addr);

        	if ((clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_len)) < 0) {
            	printf("Error listening socket: %d\n", errno);
            	exit(0);
        	}

        	char buf[__MAX_CMD_SIZE__ + 1];
        	size_t numread = read(clnt_sock, buf, __MAX_CMD_SIZE__);
//			printf("Received cmd: %s\n", buf);

        	if (numread < 0) {
            	printf("Error reading from server\n");
            	break;
        	}

        	buf[numread] = '\0';

			struct cmd_out output = exec_cmd(buf, numread);
//			printf("here result: %s\n", output.out);
        	write(clnt_sock, output.out, output.nbytes); // change size here
			close(clnt_sock);
    	}
	}
	else {
		// shell

		run_shell(__SERVER_PORT__);
	}
}

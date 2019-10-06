#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define EXIT_CMD "exit"
#define MAX_CMD_SIZE 1024
#define MAX_RESPONSE_SIZE 1024

void run_shell (int serv_port) {
	while (1) {
		printf("Enter shell commands\n");
		printf("> ");

		char *cmd = (char *) malloc(sizeof(char) * MAX_CMD_SIZE);
		size_t size = MAX_CMD_SIZE;
		size_t numread = getline(&cmd, &size, stdin);

		int con_fd = clnt_side_setup(serv_port);
		//
		// send input command to the server
		write(con_fd, cmd, numread);

		// read output of command from the server
		char buf[MAX_RESPONSE_SIZE + 1];
		size_t num_read_server = read(con_fd, buf, MAX_RESPONSE_SIZE);

		if (num_read_server < 0) {
			printf("Error reading from server\n");
			break;
		}

		buf[num_read_server] = '\0';

		printf("%s\n", buf);
	}
}

int main () {
	pid_t pid;

	if ((pid = fork()) == 0) {
		// tcp communication

		serv_side_setup(8080); // here client acts as the server


		char cmd_inp[] = "cd ..%";
		int cmd_inp_size = 6;
		cmd_inp[5] = 0;

		char *cmd, *inp;

		cmd = cmd_inp;
		inp = cmd_inp;

		int i = 0;
		while (cmd_inp[i] != 0) {
			inp++;
			i++;
		}

		inp++;

		if (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
			// cd command

			char *path = cmd + 3;
			p
			if (chdir(path) < 0) {
				printf("Error changing directory\n");
			}
		}
		else {
			int p[2];
			pipe(p);

			write(p[1], inp, cmd_inp_size - i - 1);

			close(0);
			dup(p[0]);

			FILE *fd = popen(cmd, "r");

			char output[MAX_RESPONSE_SIZE];
			int numread = read(fileno(fd), output, MAX_RESPONSE_SIZE);
			output[numread] = 0;

			printf("x: %s\n", output);
		}
	}
	else {
		// shell

		run_shell(8080);
	}
}

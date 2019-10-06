#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define EXIT_CMD "exit"
#define MAX_CMD_SIZE 1024
#define MAX_RESPONSE_SIZE 1024

int main () {
	pid_t pid;

	if ((pid = fork()) == 0) {
		// tcp communication

		struct sockaddr_in serv_addr;
		bzero(&serv_addr, sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(8080);

		int fd = socket(AF_INET, SOCK_STREAM, 0);

		int connect_res = connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (connect_res != -1) {
			while (1) {
				char buf[MAX_RESPONSE_SIZE + 1];
				size_t numread = read(fd, buf, MAX_RESPONSE_SIZE);

				if (numread < 0) {
					printf("Error reading from server\n");
					break;
				}

				buf[numread] = '\0';

				printf("\nReceived from server: %s\n", buf);
				write(1, "> ", 3);
			}
		}

	}
	else {
		// shell

		while (1) {
			printf("Enter shell commands\n");
			printf("> ");

			char *cmd = (char *) malloc(sizeof(char) * MAX_CMD_SIZE);
			size_t size = MAX_CMD_SIZE;
			size_t numread = getline(&cmd, &size, stdin);

			printf("\nCommand entered: %s\n", cmd);

		}
	}
}

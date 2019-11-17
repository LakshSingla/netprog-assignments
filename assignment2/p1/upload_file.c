#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>

#include "constants.h"

void upload_file (char *filepath, int confd) {
	int fd = open(filepath, O_RDONLY);
	if (fd == -1) {
		perror("Error opening upload file");
		exit(0);
	}

	// get file size
	struct stat file_stat;
	fstat(fd, &file_stat);
	long file_size = file_stat.st_size;
	
	// write file to server
	char buf[file_size];
	int nb = read(fd, buf, file_size);

	if (nb < 0) {
		perror("Error reading upload file");
		exit(0);
	}
	else {
		buf[nb] = 0;
		char msg[nb + 10];
		sprintf(msg, "%d#%s", nb, buf);
		int msg_size = strlen(msg) * sizeof(char);
		int n = write(confd, msg, msg_size);
		if (n != msg_size) {
			perror("Error uploading file to server");
			exit(0);
		}
	}
}

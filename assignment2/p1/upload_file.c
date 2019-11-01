#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<fcntl.h>
#include<sys/stat.h>

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
		int n = write(confd, buf, nb);
		if (n != nb) {
			perror("Error uploading file to server");
			exit(0);
		}
	}
}

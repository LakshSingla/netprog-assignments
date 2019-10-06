#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"

char **read_config(const char *filename) {
	char **arr = malloc(sizeof(char *) * (__MAX_CLIENTS__ + 1));
	FILE *fp = fopen(filename, "r");
	if(fp == NULL) {
		printf("Error opening the config file. Exiting...\n");	
		exit(0);
	}
	char name_buf[20], ip_buf[20];
	int i = 0;
	while((fscanf(fp, " %s", name_buf)) != EOF) {
			fscanf(fp, " %s", ip_buf);
			arr[i++] = strdup(ip_buf);
	}
	arr[i] = NULL;

	return arr;
}

void CONFIG_FREE(char **arr) {
	char **x = arr;
	while(*x) {
		free(*x);
		++x;
	}
	free(arr);
}

/*int main() {
	char **x = read_config("CONFIG");
	char **x2 = x;

	while(*x) {
		printf("%s\n", *x);
		++x;	
	}

	CONFIG_FREE(x2);
}*/

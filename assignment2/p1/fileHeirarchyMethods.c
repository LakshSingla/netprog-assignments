#include "fileHeirarchyMethods.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "fileNameServer.h"

char *fhm_constructpath() {
	char *path = malloc(sizeof(char) * __MAX_PATH_LEN__);
	strcat(path, __FHM_ROOTDIR__);
	for(int i = 0; i < working_len; ++i) {
		strcat(path, "/");	
		strcat(path, working_dir[i]);	
	}
	return path;
}

bool fhm_mkdir(const char *path_name) {
	char *full_path = strcat(fhm_constructpath(), "/");
	strcat(full_path, path_name);
	if(mkdir(full_path, 0777) != -1) {
		strcat(full_path, "/");
		strcat(full_path, __FHM_METAFILE__);
		if(creat(full_path, 0777) != -1) return true;
		return false;
	}
	else {
		perror("mkdir(): ");	
		return false;
	}
}

bool fhm_cd(const char *path_name) {
	//printf("%s\n", fhm_constructpath());
	DIR *curdir = opendir(fhm_constructpath());
	if(curdir == NULL) {
		perror("opendir(): ");
		return false;
	}
	struct dirent *direntry;
	bool found = false;
	while((direntry = readdir(curdir)) != NULL) {
		if(direntry->d_type == DT_DIR && strcmp(direntry->d_name, path_name) == 0) found = true;
	}
	if(!found) {
		printf("%s doesn't exist in current directory\n", path_name);
		return false;
	}
	working_dir[working_len] = strdup(path_name);
	++working_len;
	return true;
}

bool fhm_add(const char *name, const char *serv_arr) {
	//printf("%s\n", fhm_constructpath());
	char *meta_file_name = strcat(fhm_constructpath(), "/");
	strcat(meta_file_name, __FHM_METAFILE__);
	//printf("\n%s\n", meta_file_name);
	FILE *fp = fopen(meta_file_name, "a+");
	if(fp == NULL) {
		printf("Unexpected error\n");
		return false;
	}
	//printf("name %s", name);
	fprintf(fp, "%s %s\n", name, serv_arr);
	fclose(fp);
	return true;
}

bool fhm_rm(const char *name) {
	char *meta_file_name = strcat(fhm_constructpath(), "/");
	strcat(meta_file_name, __FHM_METAFILE__);
	char sed_command[__MAX_PATH_LEN__ + 20];
	sprintf(sed_command, "sed -i /\\[%s \\]/ %s", name, meta_file_name);
	//printf("\n\nCMD: %s CMD_END\n\n", sed_command);
	system(sed_command);	
	return true;
}

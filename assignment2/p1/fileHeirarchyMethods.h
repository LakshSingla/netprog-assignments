#ifndef _FILE_HEIRARCHY_METHODS_
#define _FILE_HEIRARCHY_METHODS_

#include <stdbool.h>

struct dir_info {
	int fd;
};

struct file_info {
	char *name;
	bool del;
};

bool fhm_mkdir(const char*);
bool fhm_cd(const char*);
bool fhm_ls(const char*);
bool fhm_add(const char*, const char*);
bool fhm_rm(const char*);


char* fhm_constructpath();

#endif

#include "parser.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"

// Dynamic memory allocation for:
// 1. string duplicate for modifying the string (TO BE DELETED AFTER USAGE)
// 2. token locations for parsing pipes (DELETED)
// 3. token locations for parsing dots (TO BE DELETED AFTER USAGE)
// 4. token locations for whole parsing (TO BE DELETED AFTER USAGE)

void PARSE_FREE(PARSE_OBJ parsed_val) {
	char ***x = parsed_val;
	while(*x) {
		free(*x);
		x++;
	}
	free(parsed_val);
}

char* PARSE_GET_KEY(PARSE_OBJ parsed_val, int i) {
	return parsed_val[i][0];
}

char* PARSE_GET_VAL(PARSE_OBJ parsed_val, int i) {
	return parsed_val[i][1];
}

bool PARSE_EMPTY(PARSE_OBJ parsed_val, int i) {
	return (parsed_val[i] == NULL);
}

PARSE_OBJ parse(char *str) {
	char *mod_str = strdup(str);

	char **indv_commands = parse_pipes(mod_str);
	PARSE_OBJ parsed_val = parse_dots(indv_commands);
	free(indv_commands);

	return parsed_val;
}


// str must not be a readonly literal
// free the returned val at the end
char** parse_pipes(char *str) {
	char **arr = malloc(sizeof(char*) * (__MAX_PIPES__ + 1));
	memset(arr, 0, sizeof(char*) * (__MAX_PIPES__ + 1));
	char *token;	
	token = strtok(str, "|");
	int i = 0;
	while(token != NULL) {
		arr[i++] = token;
		token = strtok(NULL, "|");
	}
	arr[i] = NULL;
	return arr;
}

// str must not be a readonly literal
char** parse_dot(char *str) {
	char **arr = malloc(sizeof(char *) * 2);
	char *ptr;
	if((ptr = strstr(str, ".")) != NULL) {
		char *token = strtok(str, ".");
		arr[0] = token;
		token = strtok(NULL, ".");
		arr[1] = token;
	} 
	else {
		arr[0] = "-";
		arr[1] = str;
	}
	return arr;
}

PARSE_OBJ parse_dots(char **strs) {
	char ***arr = malloc(sizeof(char**) * (__MAX_PIPES__ + 1));
	memset(arr, 0, sizeof(char**) * (__MAX_PIPES__ + 1));
	int i = 0;
	while(*strs) {
		arr[i] = parse_dot(*strs);
		//printf("%s: %s\n", arr[i][0], arr[i][1]);
		++i;
		++strs;
	}
	arr[i] = NULL;
	return arr;
}

/*int main() {
	PARSE_OBJ pcmd = parse("n1.cat file|n3.sort|uniq|n3.fuck off");
	int i = 0;
	while(!PARSE_EMPTY(pcmd, i)) {
		printf("%d. %s: %s\n", i, PARSE_GET_KEY(pcmd, i), PARSE_GET_VAL(pcmd, i));
		++i;
	}
	PARSE_FREE(pcmd);
}*/

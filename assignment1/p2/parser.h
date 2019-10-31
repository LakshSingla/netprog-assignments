#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>

typedef char*** PARSE_OBJ;

PARSE_OBJ parse(char*);
char** parse_pipes(char*);
char** parse_dot(char*);
PARSE_OBJ parse_dots(char**);

char* PARSE_GET_KEY(PARSE_OBJ, int);
char* PARSE_GET_VAL(PARSE_OBJ, int);
bool PARSE_EMPTY(PARSE_OBJ, int);
void PARSE_FREE(PARSE_OBJ);


#endif

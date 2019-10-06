#ifndef _PARSER_H_
#define _PARSER_H_

typedef char*** PARSE_OBJ;

char** parse_pipes(char *str);
char** parse_dot(char *str);
PARSE_OBJ parse_dots(char **strs);

char* PARSE_GET_KEY(PARSE_OBJ parsed_val, int i);
char* PARSE_GET_VAL(PARSE_OBJ parsed_val, int i);
bool PARSE_EMPTY(PARSE_OBJ parsed_val, int i);
void PARSE_FREE(PARSE_OBJ parsed_val);


#endif

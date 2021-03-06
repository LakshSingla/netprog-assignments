#ifndef _PARSE_CMD_H_
#define _PARSE_CMD_H_

#include<stdbool.h>
struct parsed_cmd {
    char ***cmd_opt_list;
    int dim;
};

typedef struct parsed_cmd PARSED_CMD;

PARSED_CMD* parse_cmd (char *cmd);
char ** check_cmd(char *in_cmd, char *req_cmd);
void FREE_CHECK_CMD_ARGS (char **args);

#endif

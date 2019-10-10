#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "parse_cmd.h"

char ** check_cmd (char *in_cmd, char *req_cmd, int req_arg_count) {
	char **args = malloc(req_arg_count * sizeof(char *));
	char *in_cmd_copy = strdup(in_cmd);
	if (req_arg_count >= 1) {
		char *token = strtok(in_cmd_copy, " ");

		if (token != NULL && strcmp(token, req_cmd) == 0) {
			int arg_count = 0;

			while (token != NULL && arg_count < req_arg_count) {
				args[arg_count] = strdup(token);
				arg_count++;
				token = strtok(NULL, " ");
			}

			if (arg_count == req_arg_count && token == NULL) {
				args[arg_count] = NULL;
				free(in_cmd_copy);
				return args;
			}
			else {
				printf("%s: command needs exactly %d arguments\n", req_cmd, req_arg_count);
			}
		}
	}

	free(in_cmd_copy);
	args[0] = NULL;
	return args;
}

void FREE_CHECK_CMD_ARGS (char **args) {
	free(args);
}

PARSED_CMD* update_parsed_cmd (PARSED_CMD prev_parsed, char *delim) {
	PARSED_CMD* new_parsed = (PARSED_CMD *) malloc(sizeof(PARSED_CMD));
	new_parsed->dim = prev_parsed.dim;

	char ***new_cmd_opt = malloc(prev_parsed.dim * sizeof(char **));

	int i = 0;
	while (i < prev_parsed.dim) {
		int new_cmd_j = 0;
		new_cmd_opt[i] = malloc((__MAX_CMD_COUNT__ + 1) * sizeof(char *));

		int j = 0;
		while (prev_parsed.cmd_opt_list[i][j] != NULL) {
			char *cmd_copy = strdup(prev_parsed.cmd_opt_list[i][j]);
			char *token = strtok(cmd_copy, delim);
			if (token != NULL) {
				while (token != NULL) {
					new_cmd_opt[i][new_cmd_j] = token;
					new_cmd_j++;
					new_cmd_opt[i][new_cmd_j] = delim;
					new_cmd_j++;

					token = strtok(NULL, delim);
				}
				new_cmd_opt[i][new_cmd_j - 1] = NULL;
				new_cmd_j--;
			}
			j++;
		}

		i++;
	}

	new_parsed->cmd_opt_list = new_cmd_opt;

	return new_parsed;
}

PARSED_CMD* parse_cmd (char *cmd) {
	PARSED_CMD init_p;
	init_p.dim = 1;
	init_p.cmd_opt_list = malloc(1 * sizeof(char **));
	init_p.cmd_opt_list[0] = malloc((__MAX_CMD_COUNT__ + 1) * sizeof(char *));

	init_p.cmd_opt_list[0][0] = cmd;
	init_p.cmd_opt_list[0][1] = NULL;

	PARSED_CMD* new_p = update_parsed_cmd(init_p, "|");
	new_p = update_parsed_cmd(*new_p, "#");
	new_p = update_parsed_cmd(*new_p, "S");

	return new_p;

/*	int x = 0;
	while (x < new_p->dim) {
		int i = 0;
		while (new_p->cmd_opt_list[x][i] != NULL) {
			printf("as: %d %s\n", i, new_p->cmd_opt_list[x][i]);
			i++;
		}
		x++;
	}*/
}

void FREE_CMD_LIST (char **cmd_list) {
	free(cmd_list);
}

/*int main () {
//	parse_cmd("a|b#c|dSq");
//	if(check_cmd("bg 123", "bg", 2)) printf("asdfasdf\n");
}*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include "constants.h"
#include "get_canonical_path.h"

int main() {
	while(true) {
		printf(__PROMPT__);	
		size_t cmd_size = __MAX_CMD_SIZE__ + 1;
		char *cmd_buf =	malloc(sizeof(char) * cmd_size);
		ssize_t cmd_size_act = getline(&cmd_buf, &cmd_size, stdin);
		if(cmd_size_act == -1 || cmd_size_act == 0) { /* error */ }
		cmd_buf[cmd_size_act-1] = 0;
		//printf("%ld, %s", cmd_size_act, cmd_buf);
		

		char *canonical_path = get_canonical_path(cmd_buf);

		if(canonical_path == NULL) {
			printf("%s: Command not found\n", cmd_buf);	
			FREE_CANONICAL_PATH(canonical_path);
			continue;
		}




		printf("%s\n", canonical_path);
		

		FREE_CANONICAL_PATH(canonical_path);
	}
}

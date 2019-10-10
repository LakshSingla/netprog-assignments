#include "proc_management.h"

#include <stdbool.h>

extern int no_procs = 0;
extern struct command *current_commands[__MAX_SIMULT_CMDS__];

// Add a dynamically allocated structure
bool add_to_group(struct command *cmd) {
	for(int i = 0; i < __MAX_SIMULT_CMDS__; ++i) {
		if(current_commands[i] == NULL) {
			current_commands[i]	= cmd;
			cmd->ref_id = i;
			return true;
		}
	}
	return false;
}

bool remove_from_group(int ref_id) {
	if(current_commands[ref_id] == NULL) return false;

	free(current_commands[ref_id]);
	current_commands[ref_id] = NULL;
	return true;
}

bool get_cmd_by_pgid(pid_t pgid) {

}

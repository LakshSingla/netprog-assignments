#include "proc_management.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "constants.h"

int no_procs = 0;
struct command *current_commands[__MAX_SIMULT_CMDS__];

// Add a dynamically allocated structure
bool add_to_group(struct command *cmd) {
	for(int i = 0; i < __MAX_SIMULT_CMDS__; ++i) {
		if(current_commands[i] == NULL) {
			current_commands[i]	= cmd;
			current_commands[i]->cmd = strdup(current_commands[i]->cmd);
			cmd->ref_id = i;
			return true;
		}
	}
	return false;
}

int get_refid_from_pgid(pid_t pgid) {
	for(int i = 0; i < __MAX_SIMULT_CMDS__; ++i) {
		if(current_commands[i]->pgid == pgid)	 return i;
	}
	return -1;
}

bool remove_from_group(int ref_id) {
	if(current_commands[ref_id] == NULL) return false;

	free(current_commands[ref_id]->cmd);
	free(current_commands[ref_id]);
	current_commands[ref_id] = NULL;
	return true;
}

bool set_cmd(int ref_id, enum TERMINAL_CONTROL tcm, enum COMMAND_RUN_MODE crm) {
	if(current_commands[ref_id] == NULL) return false;
	current_commands[ref_id]->tcm = tcm;
	current_commands[ref_id]->crm = crm;
	return true;
}

bool set_cmd_by_pgid(pid_t pgid, enum TERMINAL_CONTROL tcm, enum COMMAND_RUN_MODE crm) {
	return set_cmd(get_refid_from_pgid(pgid), tcm, crm);
}

bool remove_from_group_by_pgid(pid_t pgid) {
	return remove_from_group(get_refid_from_pgid(pgid));
}

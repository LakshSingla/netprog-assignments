#ifndef _PROC_MANAGEMENT_H_
#define _PROC_MANAGEMENT_H_

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"

enum TERMINAL_CONTROL {TC_FG, TC_BG};
enum COMMAND_RUN_MODE {CRM_RUN, CRM_STP};

struct command {
	int ref_id;
	char *cmd;
	pid_t pgid;
	enum TERMINAL_CONTROL tcm; 
	enum COMMAND_RUN_MODE crm;
};

extern struct command *current_commands[__MAX_SIMULT_CMDS__];
extern int no_procs;

bool add_to_group(struct command *);
int get_refid_from_pgid(pid_t );
bool remove_from_group(int );
bool set_cmd(int , enum TERMINAL_CONTROL , enum COMMAND_RUN_MODE );
bool set_cmd_by_pgid(pid_t , enum TERMINAL_CONTROL , enum COMMAND_RUN_MODE );
bool remove_from_group_by_pgid(pid_t );


#endif

#ifndef _PROC_MANAGEMENT_H_
#define _PROC_MANAGEMENT_H_

enum TERMINAL_CONTROL {TC_FG, TC_BG};
enum COMMAND_RUN_MODE {CRM_RUN, CRM_STP};

struct command {
	int ref_id;
	char *cmd;
	pid_t pgid;
	enum TERMINAL_CONTROL tcm; // 0 -> fg, 1 -> bg
	enum COMMAND_RUN_MODE crm; //0 -> running, 1 -> stopped
};

extern int no_procs;
extern struct command **current_commands;


#endif

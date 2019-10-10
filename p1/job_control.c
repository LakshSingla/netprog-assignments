#include "job_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "constants.h"
#include "proc_management.h"

void bg(int ref_id) {
	if(current_commands[ref_id] == NULL) {
		printf("Reference ID not found\n");
		exit(-1);	
	}
	if(current_commands[ref_id]->tcm != TC_BG) {
		printf("Process not running in background. Unexpected behaviour...\n");
		exit(-1);	
	}
	if(kill(current_commands[ref_id]->pgid, SIGCONT) < 0) {
		printf("Unable to restart the process\n");	
		exit(-1);
	}
	set_cmd(ref_id, TC_BG, CRM_RUN);
}

void fg(int ref_id) {
	if(current_commands[ref_id] == NULL) {
		printf("Reference ID not found\n");
		exit(-1);	
	}
	if(current_commands[ref_id]->tcm != TC_BG) {
		printf("Process not running in background. Unexpected behaviour...\n");
		exit(-1);	
	}
	
	set_cmd(ref_id, TC_FG, CRM_RUN);
	int child_executer_status;
	printf("here1\n");
//	signal(SIGTTOU, SIG_IGN);
	if(kill(current_commands[ref_id]->pgid, SIGCONT) < 0) {
		printf("Unable to restart the process\n");	
		exit(-1);
	}
/*	if(tcsetpgrp(0, current_commands[ref_id]->pgid) == -1) {
			printf("Unable to set process group as foreground. Exiting command...\n");
			exit(0);
	}
	printf("here2\n");
	int _pgid = current_commands[ref_id]->pgid;
	printf("herex: %d\n", _pgid);
	do {
		waitpid(_pgid, &child_executer_status, WUNTRACED);
	} while (
		!WIFSIGNALED(child_executer_status)			&&
		!WIFEXITED(child_executer_status)				&&
		!WIFSTOPPED(child_executer_status)
	);
	if(WIFSTOPPED(child_executer_status)) {
		printf("stopped again\n");
		set_cmd_by_pgid(_pgid, TC_BG, CRM_STP);
	}
			 	// Foreground process gets terminated
	else if(WIFEXITED(child_executer_status) || WIFSIGNALED(child_executer_status)) {
		if(WIFSIGNALED(child_executer_status)) {
			printf("signl again - %d\n", WTERMSIG(child_executer_status));
		}
		printf("signl/exit again\n");
		remove_from_group_by_pgid(_pgid);
	 }
	signal(SIGTTOU, SIG_IGN);
	if(tcsetpgrp(0, getpgid(0)) < 0) {
		printf("Unable to reclaim the foreground process. Exiting...\n");	
		exit(-1);
	}
	signal(SIGTTOU, SIG_DFL);
	printf("completeo\n");*/
}

void print_details() {
	printf("key: [REF_ID]||COMMAND_NAME||{GROUP_ID}||STATUS\n");
	for(int i = 0; i < __MAX_SIMULT_CMDS__; ++i) {
		if(current_commands[i] == NULL) continue;
		struct command *tmp = current_commands[i];
		printf("[%d]||%s||{%d}||%d\n", tmp->ref_id, tmp->cmd, tmp->pgid,  tmp->crm);
	}
}

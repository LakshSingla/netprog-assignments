#include "job_control.h"

#include <stdlib.h>

#include "proc_management.h"

void bg() {
	
}

void fg() {

}

void print_details() {
	printf("REF_ID\t\tCOMMAND_NAME\t\tGROUP_ID\t\tSTATUS\n");
	for(int i = 0; i < __MAX_SIMULT_CMDS__; ++i) {
		if(current_commands[i] == NULL) continue;
		struct command *tmp = current_commands[i];
		printf("%d\t\t%s\t\t%ld\t\t%d\n", tmp->ref_id, ref->cmd, ref->pgid,  ref->run_mode);
	}
}

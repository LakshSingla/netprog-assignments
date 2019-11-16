#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define __MAX_CMD_SIZE__	1024
#define __MAX_PENDING__		1024
#define __ONE_MB__		1024
#define __PROMPT__		"bigfs> "
#define __NAME_SERVER_PORT__	4000	

// client commands
#define __UPLOAD_CMD__		"upload"
#define __UPLOAD_CODE__		"UP"
#define __LS_CMD__		"ls"
#define __LS_CODE__		"LS"
#define __CP_CMD__		"cp"
#define __CP_CODE__		"CP"
#define __MV_CMD__		"mv"
#define __MV_CODE__		"MV"
#define __MKDIR_CMD__		"mkdir"
#define __MKDIR_CODE__		"MK"
#define __RM_CMD__		"rm"
#define __RM_CODE__		"RM"
#define __CAT_CMD__		"cat"
#define __CAT_CODE__		"CT"
#define __CMD_CODE_LEN__	2

// statuses
#define __INIT_STATUS__		0
#define __CMD_READ__		1
#define __UPLOADING_FILE__	2
#endif

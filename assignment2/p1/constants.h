#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define __MAX_CMD_SIZE__	1024
#define __MAX_PENDING__		1024
#define __ONE_MB__		1024
#define __PROMPT__		"bigfs> "
#define __NAME_SERVER_IP__	"127.0.0.1"
#define __NAME_SERVER_PORT__	4000	
#define __MAX_PATH_COMPONENTS__	100
#define __MAX_PATH_LEN__	400
#define __FHM_METAFILE__	"fhm.meta"
#define __FHM_ROOTDIR__		"fhm.root"
#define __MAX_RESP_SIZE__	50

// client commands
#define __UPLOAD_CMD__		"upload"
#define __UPLOAD_CODE__		"UP"
#define __LS_CMD__		"ls"
#define __LS_CODE__		"LS"
#define __CP_CMD__		"cp"
#define __CP_CODE__		"CP"
#define __CD_CMD__		"cd"
#define __CD_CODE__		"CD"
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
#define __FILE_UPLOADED__	3

#endif

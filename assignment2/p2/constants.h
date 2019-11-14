#define __PROMPT__ 			"> "
#define __DFL_PORT1__			5000
#define __DFL_PORT2__			6000
#define __DFL_PORT3__			7000
#define __MAX_PENDING__			1024
#define __MAX_CONFIG_SIZE__		50
#define __MAX_MSG_SIZE__		512
#define __MAX_MSG_COUNT__		100
#define __MAX_TOPIC_SIZE__		20
#define __MAX_TOPIC_COUNT__		50
#define __MAX_RESPONSE_SIZE__		__MAX_MSG_SIZE__
#define __MAX_CMD_SIZE__		__MAX_MSG_SIZE__ + __MAX_TOPIC_SIZE__ + 10


#define __SUB_CMD__			"subscribe"
#define __SUB_SUB_CODE__		"0"
#define __SUB_RET_CMD__			"retrieve"
#define __SUB_RET_CODE__		"1"
#define __SUB_RETALL_CMD__		"retrieve-all"
#define __SUB_RETALL_CODE__		"2"

#define __PUB_CREATE_CMD__		"create"
#define __PUB_CREATE_CODE__		"0"
#define __PUB_SEND_CMD__		"send"
#define __PUB_SEND_CODE__		"1"
#define __PUB_SENDFILE_CMD__		"send-file"

#define __BROK_CLASS__			"BRO"
#define __SUB_CLASS__			"SUB"
#define __PUB_CLASS__			"PUB"
#define __CLASS_LEN__			4

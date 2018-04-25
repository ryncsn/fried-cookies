#include <sys/socket.h>
#include <netinet/in.h>

typedef struct basic_block_t
{
	int type;
	int size;
	char content[];
} basic_block;

#define block_head sizeof(int)*2
#define sizeof_basic_block(X)\
	(sizeof(int)+sizeof(int)+sizeof(char)*X)

#define basic_block_data_length(X)\
	(X-sizeof(int)*2)

#define COMMAND_FLAG  0x10000000
#define	CMD_LS        0x10000001
#define	CMD_RM        0x10000002
#define	CMD_UP        0x10000004
#define	CMD_DOWN      0x10000008
#define	CMD_RESUME    0x1000000F
#define	CMD_PAUSE     0x10000010
#define	CMD_FINISH    0x10000020

#define DATA_BLOCK    0x20000000
#define ERROR_BLOCK   0x40000000
#define MSG_BLOCK     0x80000000

/*INTERNAL COMMANDS*/

#define LIST_FILE 0xF0000001
#define UPLOAD    0xF0000002
#define DOWNLOAD  0xF0000004
#define PAUSE     0xF0000008
#define CONTINUE  0xF000000F
#define MSGING    0xF0000010
#define EXITING   0xF000FFFF

#define MAXLINE 4096
#define DEFLINE 1300
#define LISTENQ 1024
#define SERV_PORT 9877


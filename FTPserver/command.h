# ifndef __COMMAND__
# define __COMMAND__

# include <string.h>
# include <stdio.h>
# include <stdbool.h>

# include "util.h"

# define CMD_NUM 7

# define CMD_GET  "get"
# define CMD_PUT  "put"
# define CMD_DEL  "delete"
# define CMD_LS   "ls"
# define CMD_CD   "cd"
# define CMD_MKD  "mkdir"
# define CMD_PWD  "pwd"

# define NULL_STR ""

# define IGNORE_ARGUMENT(cmd) #cmd " command should not have args. Redundant args are ignored.\n"

# define HELP_GET "get [remote_filename] Copy file with the name [remote_filename] \
from remote directory to local directory."
# define HELP_PUT "put [local_filename] Copy file with the name [local_filename] \
from local directory to remote directory."
# define HELP_DEL "delete [remote_filename] Delete the file with the name [remote_filename] \
from the remote directory."
# define HELP_LS "List the files and subdirectories in the remote directory."
# define HELP_CD "cd [remote_direcotry] Change to the [remote_direcotry] \
on the remote machine or change to the parent directory of the current directory."
# define HELP_MKD "mkdir[remote_direcotry] Create directory named [remote_direcotry] \
as the sub-directory of the current working directory on the remote machine."
# define HELP_PWD "Print the current working directory on the remote machine."

typedef enum {
    SEM_GET, 
    SEM_PUT, 
    SEM_DEL, 
    SEM_LS,
    SEM_CD,
    SEM_MKD,
    SEM_PWD,
    SEM_QUIT,
    SEM_ERR
} Semanteme;


typedef enum {
    FILENAME, FAILDFIND, FILESIZE, READYDOWN, FILEPACKET, SUCCESS, FAILD, END,CANCEL, DOWNLOADING
}Msg;

#pragma pack(1)
typedef struct {
    Msg msg;
    union MyUnion {
        struct {
            int fileSize;
            char fileName[MAX_LEN - sizeof(int) * 2];
        }fileInfo;
        struct {
            int dataBufSize;
            int offset;
            char dataBuf[MAX_LEN - sizeof(int) * 3];
        }dataInfo;
    };
}Packet;
#pragma pack()

typedef struct{
    Semanteme sem;
    short arg_num;
    const char* cmd;
    const char* help;
    bool (*function)(SOCKET, const char*);
} Command;

bool server_get(SOCKET, const char*);
bool server_put(SOCKET, const char*);
bool server_delete(SOCKET, const char*);
bool server_ls(SOCKET ,const char*);
bool server_cd(SOCKET , const char*);
bool server_mkdir(SOCKET , const char*);
bool server_pwd(SOCKET , const char*);

int download(SOCKET, Packet*);


int uploadToC(SOCKET s, Packet* packet);
Semanteme get_semanteme(char*);
Command get_command(const char*);

extern const Command commands[CMD_NUM];
extern const Command error_command;
extern const Command null_command;

# endif
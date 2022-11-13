# ifndef __COMMAND__
# define __COMMAND__

# include <string.h>
# include <stdio.h>
# include <stdbool.h>

# include "util.h"

# define CMD_NUM 8

# define CMD_GET  "get"
# define CMD_PUT  "put"
# define CMD_DEL  "delete"
# define CMD_LS   "ls"
# define CMD_CD   "cd"
# define CMD_MKD  "mkdir"
# define CMD_PWD  "pwd"
# define CMD_QUIT "quit"

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
# define HELP_QUIT "End the FTP session."

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

typedef struct{
    Semanteme sem;
    short arg_num;
    const char* cmd;
    const char* help;
    bool (*function)(sockaddr_in, const char*);
} Command;

bool client_get(sockaddr_in, const char*);
bool client_put(sockaddr_in, const char*);
bool client_delete(sockaddr_in, const char*);
bool client_ls(sockaddr_in, const char*);
bool client_cd(sockaddr_in, const char*);
bool client_mkdir(sockaddr_in, const char*);
bool client_pwd(sockaddr_in, const char*);
bool client_quit(sockaddr_in, const char*);
bool client_err(sockaddr_in, const char*);
bool client_null(sockaddr_in, const char*);

void help();

Semanteme get_semanteme(char*);
Command get_command(const char*);

extern const Command commands[CMD_NUM];
extern const Command error_command;
extern const Command null_command;

# endif
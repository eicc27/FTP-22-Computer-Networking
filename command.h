# ifndef __COMMAND__
# define __COMMAND__

# include <string.h>
# include <stdio.h>
# include <stdbool.h>

# define CMD_NUM 8

# define CMD_GET  "get"
# define CMD_PUT  "put"
# define CMD_DEL  "delete"
# define CMD_LS   "ls"
# define CMD_CD   "cd"
# define CMD_MKD  "mkdir"
# define CMD_PWD  "pwd"
# define CMD_QUIT "quit"

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
    bool (*function)(const char*);
} Command;

bool client_get(const char*);
bool client_put(const char*);
bool client_delete(const char*);
bool client_ls(const char*);
bool client_cd(const char*);
bool client_mkdir(const char*);
bool client_pwd(const char*);
bool client_quit(const char*);
bool client_err(const char*);

Semanteme get_semanteme(char*);
Command get_command(char*);

extern const Command commands[CMD_NUM];
extern const Command error_command;

# endif
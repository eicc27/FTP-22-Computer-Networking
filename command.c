# include "command.h"

// TODO: implement functions client_xxx

const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, "", client_get},
    {SEM_PUT, 1, CMD_PUT, "", client_put},
    {SEM_DEL, 1, CMD_DEL, "", client_delete},
    {SEM_LS, 0, CMD_LS, "", client_ls},
    {SEM_CD, 1, CMD_CD, "", client_cd},
    {SEM_MKD, 1, CMD_MKD, "", client_mkdir},
    {SEM_PWD, 0, CMD_PWD, "", client_pwd},
    {SEM_QUIT, 0, CMD_QUIT, "", client_quit}    
};

const Command error_command = {SEM_ERR, 0, "", "", client_err};

Semanteme get_semanteme(char* cmd){
    for(int i = 0; i < CMD_NUM; i++)
        if(!strcmp(cmd, commands[i].cmd)) return commands[i].sem;
    return SEM_ERR;
}


Command get_command(char* cmd){
    for(int i = 0; i < CMD_NUM; i++)
        if(!strcmp(cmd, commands[i].cmd)) return commands[i];
    return error_command;
}

bool client_get(const char* arg){
    printf("client get %s\n", arg);
    return true;
}

bool client_put(const char* arg){
    printf("client put %s\n", arg);
    return true;
}

bool client_delete(const char* arg){
    printf("client delete %s\n", arg);
    return true;
}

bool client_ls(const char* arg){ // arg not used
    if(arg) printf("ls command should not have args. Redundant args are ignored.\n");
    printf("client ls %s\n", arg);
    return true;
}

bool client_cd(const char* arg){
    printf("client cd %s\n", arg);
    return true;
}

bool client_mkdir(const char* arg){
    printf("client mkdir %s\n", arg);
    return true;
}

bool client_pwd(const char* arg){ // arg not used
    if(arg) printf("pwd command should not have args. Redundant args are ignored.\n");
    printf("client pwd\n");
    return true;
}

bool client_quit(const char* arg){ // arg not used
    if(arg) printf("quit command should not have args. Redundant args are ignored.\n");
    printf("leaving ftp...\n");
    return false;
}

bool client_err(const char* arg){
    return true;
}
# include "command.h"

// TODO: implement functions client_xxx


const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, HELP_GET, client_get},
    {SEM_PUT, 1, CMD_PUT, HELP_PUT, client_put},
    {SEM_DEL, 1, CMD_DEL, HELP_DEL, client_delete},
    {SEM_LS, 0, CMD_LS, HELP_LS, client_ls},
    {SEM_CD, 1, CMD_CD, HELP_CD, client_cd},
    {SEM_MKD, 1, CMD_MKD, HELP_MKD, client_mkdir},
    {SEM_PWD, 0, CMD_PWD, HELP_PWD, client_pwd},
    {SEM_QUIT, 0, CMD_QUIT, HELP_QUIT, client_quit}    
};

const Command error_command = {SEM_ERR, 0, NULL_STR, "", client_err};
const Command null_command = {SEM_ERR, 0, NULL_STR, "", client_null};

Semanteme get_semanteme(char* cmd){
    if(!strcmp(cmd, NULL_STR)) return SEM_ERR;
    for_i_in_range(CMD_NUM)
        if(!strcmp(cmd, commands[i].cmd)) return commands[i].sem;
    return SEM_ERR;
}


Command get_command(const char* cmd){
    if(!strcmp(cmd, NULL_STR)) return null_command;
    for_i_in_range(CMD_NUM)
        if(!strcmp(cmd, commands[i].cmd)) return commands[i];
    return error_command;
}

bool client_get(const char* arg){
    // TODO: client get
    printf("client get %s\n", arg);

    return true;
}

bool client_put(const char* arg){
    // TODO: client put
    printf("client put %s\n", arg);

    return true;
}

bool client_delete(const char* arg){
    // TODO: client delete
    printf("client delete %s\n", arg);

    return true;
}

bool client_ls(const char* arg){ // arg not used
    // TODO: client ls
    if(arg) printf(IGNORE_ARGUMENT(ls));
    printf("list in dir\n");

    return true;
}

bool client_cd(const char* arg){
    // TODO: client cd
    printf("client cd %s\n", arg);

    return true;
}

bool client_mkdir(const char* arg){
    // TODO: client mkdir
    printf("client mkdir %s\n", arg);

    return true;
}

bool client_pwd(const char* arg){ // arg not used
    // TODO: client pwd
    if(arg) printf(IGNORE_ARGUMENT(pwd));
    printf("client pwd\n");
    
    return true;
}

bool client_quit(const char* arg){ // arg not used
    if(arg) printf(IGNORE_ARGUMENT(quit));
    printf("leaving ftp...\n");
    // TODO: close socket
    return false;
}

bool client_err(const char* arg){
    printf("invalid command\n");
    help();
    return true;
}

bool client_null(const char* arg){
    return true;
}

void help(){
    for_i_in_range(CMD_NUM)
        printf("%8s -- %s\n", commands[i].cmd, commands[i].help);
}
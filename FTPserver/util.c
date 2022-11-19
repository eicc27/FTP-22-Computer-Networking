# include "util.h"

void clear_buffer(){
    while(getchar()!='\n');
}

Arguments split_string(const char* input){
    const char* p = NULL;
    Arguments args = {0, 0};

    if(!input || !strlen(input)){
        args.comment = "input is null!";
        printf("input is null!");
        args.err = true;
        return args;
    }
    
    p = strtok((char*)input, " ");
    args.argv[0] = p;
    args.argc ++;
    while((p = strtok(NULL, " "))!=NULL){
        args.argv[args.argc] = p;
        args.argc++;
    }

    return args;
}



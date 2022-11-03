# include "util.h"

// TODO: implement functions in util.h
void clear_buffer(){
    while(getchar()!='\n');
}

Arguments split_input(const char* input){
    const char* p = NULL;
    Arguments args = {0, 0};

    if(!input || !strlen(input)){
        args.comment = "input is null!";
        printf("input is null!");
        args.err = true;
        return args;
    }

    p = strtok(input, " ");
    // printf("%s\n", p);
    args.argv[0] = p;
    args.argc ++;
    while((p = strtok(NULL, " "))!=NULL){
        args.argv[args.argc] = p;
        args.argc++;
    }

    return args;
}
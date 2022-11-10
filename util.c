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

int new_socket_conn(sockaddr_in addr){



    // under linux
    int sockfd;
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    return sockfd;
}

void close_socket_conn(int sockfd){

    // under linux
    close(sockfd);
}
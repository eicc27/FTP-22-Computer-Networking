# include <stdio.h>
# include "command.h"
# include "util.h"

sockaddr_in initialize(){
    printf("initializing ftp...\n");
    sockaddr_in addr;

    memset(&addr, 0,sizeof(addr));     //将＆addr中的前sizeof（addr）字节置为0，包括'\0'
    addr.sin_family = AF_INET;      //AF_INET代表TCP／IP协议
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //将点间隔地址转换为网络字节顺序
    addr.sin_port = htons(8989);    //转换为网络字节顺序


    return addr;
}

// Read command lines and react.
bool client_ftp(sockaddr_in addr){
    printf("ftp>");
    char input[MAX_LEN];
    scanf(" %256[^\n]", input);
    clear_buffer();
    Arguments args = split_string(input);
    Command cmd = get_command(args.argv[0]);
    return cmd.function(addr, args.argc > 1 ? args.argv[1] : NULL);
}


int main(int argc, char* argv[]){
    sockaddr_in addr = initialize();
    printf("%d", addr);
    while(client_ftp(addr));
}
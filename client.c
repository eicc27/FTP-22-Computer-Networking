# include <stdio.h>
# include "command.h"
# include "util.h"

# define MAX_LEN 256

void initialize(){
    // TODO: initialize the ftp client end
    return;
}


// Read command lines and react.
bool client_ftp(){
    printf(">");
    char input[MAX_LEN];
    scanf(" %[^\n]", input);

    Arguments args = split_input(input);

    Command cmd = get_command(args.argv[0]);

    // printf("checking %d\n", args.argc);
    // for(int i = 0; i < args.argc; i++) printf("%s\n", args.argv[i]);

    bool res = cmd.function(args.argv[1]);

    clear_buffer();
    cmd = null_command;

    return res;
}


int main(int argc, char* argv[]){
    initialize();
    while(client_ftp());
}
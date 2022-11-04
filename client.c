# include <stdio.h>
# include "command.h"
# include "util.h"

void initialize(){
    // TODO: initialize the ftp client end
    printf("initializing ftp...\n");

    return;
}

// Read command lines and react.
bool client_ftp(){
    printf("ftp>");
    char input[MAX_LEN];
    scanf(" %256[^\n]", input);
    clear_buffer();
    Arguments args = split_string(input);
    Command cmd = get_command(args.argv[0]);
    return cmd.function(args.argc>1 ? args.argv[1] : NULL);
}


int main(int argc, char* argv[]){
    initialize();
    while(client_ftp());
}
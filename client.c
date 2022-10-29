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
    char input[MAX_LEN];
    scanf("%[^\n]", input);

    // TODO: split string
    Command cmd = get_command(input); // FIXME
    printf("Hello World! %d\n", cmd.sem);

    bool res = cmd.function(NULL);
    clear_buffer();

    return res;
}


int main(){
    initialize();
    while(client_ftp());
}
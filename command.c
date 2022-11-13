#include "command.h"
#include "util.h"
#include <fcntl.h>

const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, HELP_GET, client_get},    {SEM_PUT, 1, CMD_PUT, HELP_PUT, client_put},
    {SEM_DEL, 1, CMD_DEL, HELP_DEL, client_delete}, {SEM_LS, 0, CMD_LS, HELP_LS, client_ls},
    {SEM_CD, 1, CMD_CD, HELP_CD, client_cd},        {SEM_MKD, 1, CMD_MKD, HELP_MKD, client_mkdir},
    {SEM_PWD, 0, CMD_PWD, HELP_PWD, client_pwd},    {SEM_QUIT, 0, CMD_QUIT, HELP_QUIT, client_quit}};

const Command error_command = {SEM_ERR, 0, NULL_STR, "", client_err};
const Command null_command = {SEM_ERR, 0, NULL_STR, "", client_null};

Semanteme get_semanteme(char *cmd)
{
    if (!strcmp(cmd, NULL_STR))
        return SEM_ERR;
    for_i_in_range(CMD_NUM) if (!strcmp(cmd, commands[i].cmd)) return commands[i].sem;
    return SEM_ERR;
}

Command get_command(const char *cmd)
{
    if (!strcmp(cmd, NULL_STR))
        return null_command;
    for_i_in_range(CMD_NUM) if (!strcmp(cmd, commands[i].cmd)) return commands[i];
    return error_command;
}

/*Run `get` command to get(download) the given data(file name) from the server.
TODO: The server must define a proper method when the given file is not found.
Here it asserts that the server returns a single 'N' character if the case happens.
*/
bool client_get(sockaddr_in addr, const char *arg)
{
    // file buffer for storing the data server sent
    char *fileBuffer = (char *)calloc(MAX_LEN, sizeof(char));
    // local file
    int fileLocal;
    // bytes num read from server
    int nbytes;
    printf("client get %s\n", arg);
    int sockfd = new_socket_conn(addr);
    // client `arg` ---> server
    if (write(sockfd, unshift_a(CMD_GET, (char *)arg), MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    // server `sockfd` ---> client
    if (read(sockfd, fileBuffer, MAX_LEN) < 0)
    {
        printf("Read error: Failed to read from server\n");
        exit(1);
    }
    if (fileBuffer[0] == 'N')
    {
        printf("Read error: The server cannot open the given file\n");
        exit(1);
    }
    if ((fileLocal = open(arg + 4, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
    {
        printf("Open error: The client cannot open local file\n");
        exit(1);
    }
    while ((nbytes = read(sockfd, fileBuffer, MAX_LEN)) > 0)
    {
        if (write(fileLocal, fileBuffer, nbytes) < 0)
        {
            printf("Write error: Error happened while writing local file\n");
            exit(1);
        }
    }
    close(fileLocal);
    close_socket_conn(sockfd);
    return true;
}

/*Run `put` command to put(upload) a given file to the server's given directory.
 */
bool client_put(sockaddr_in addr, const char *arg)
{
    printf("client put %s\n", arg);
    int sockfd = new_socket_conn(addr);
    // client `arg` ---> server
    if (write(sockfd, unshift_a(CMD_PUT, (char *)arg), MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    // open the new local file
    int fileLocal;
    if ((fileLocal = open(arg + 4, O_RDONLY)) < 0)
    {
        printf("Open error: unable to open local file\n");
        exit(-1);
    }
    // read N bytes from `fileLocal`...
    int nbytes;
    char *fileBuffer = (char *)calloc(MAX_LEN, sizeof(char));
    while ((nbytes = read(fileLocal, fileBuffer, MAX_LEN)) > 0)
    {
        // ...and write into the socket
        if (write(sockfd, fileBuffer, nbytes) < 0)
        {
            printf("Write error: Failed to write into socket\n");
            return (-1);
        }
    }
    close_socket_conn(sockfd);
    return true;
}

/*Run `delete` to remove a given directory on the server.
The client just simply prints the value the server returns.
 */
bool client_delete(sockaddr_in addr, const char *arg)
{
    int sockfd = new_socket_conn(addr);
    // client arg ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, (void *)arg, MAX_LEN) > 0)
        printf("%s", arg);
    close_socket_conn(sockfd);
    return true;
}

/*Run `ls` command to list the given directory on the server, which then returns the listed directory.
The client just simply prints the value the server returns.
TODO: This interface sends the argument directly to the server.
The server has to deal with and parse these different arguments.
*/
bool client_ls(sockaddr_in addr, const char *arg)
{
    if (arg)
        printf(IGNORE_ARGUMENT(ls));
    int sockfd = new_socket_conn(addr);
    // client `ls`---> server
    if (write(sockfd, CMD_LS, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("list in dir...\n");
    // server `sockfd` ---> arg
    while (read(sockfd, (void *)arg, MAX_LEN) > 0)
        printf("%s", arg);
    close_socket_conn(sockfd);
    return true;
}

/*Run `cd` to change into a different server's directory.
The client just simply prints the value the server returns.
TODO: Like powershell on Windows, `cd` changes the 'prefix'
of the client interface. Just like:
C:/Program Files>cd ..
C:/>
*/
bool client_cd(sockaddr_in addr, const char *arg)
{
    printf("client cd %s\n", arg);
    int sockfd = new_socket_conn(addr);
    // client arg ---> server
    if (write(sockfd, unshift_a(CMD_CD, (char *)arg), MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, (void *)arg, MAX_LEN) > 0)
        printf("%s", arg);
    close_socket_conn(sockfd);
    return true;
}

/*Run `mkdir` to make a new directory on client.
The client just simply prints the value the server returns.
*/
bool client_mkdir(sockaddr_in addr, const char *arg)
{
    printf("client mkdir %s\n", arg);
    int sockfd = new_socket_conn(addr);
    // client arg ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, (void *)arg, MAX_LEN) > 0)
        printf("%s", arg);
    close_socket_conn(sockfd);
    return true;
}

/* Run `pwd` to print the current working directory on server.
 */
bool client_pwd(sockaddr_in addr, const char *arg)
{
    if (arg)
        printf(IGNORE_ARGUMENT(pwd));
    int sockfd = new_socket_conn(addr);
    // client arg ---> server
    if (write(sockfd, CMD_PWD, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, (void *)arg, MAX_LEN) > 0)
        printf("%s", arg);
    printf("client pwd\n");
    return true;
}

bool client_quit(sockaddr_in addr, const char *arg)
{ // arg not used
    if (arg)
        printf(IGNORE_ARGUMENT(quit));
    printf("leaving ftp...\n");
    return false;
}

bool client_err(sockaddr_in addr, const char *arg)
{
    printf("invalid command\n");
    help();
    return true;
}

bool client_null(sockaddr_in addr, const char *arg)
{
    return true;
}

void help()
{
    for_i_in_range(CMD_NUM) printf("%8s -- %s\n", commands[i].cmd, commands[i].help);
}
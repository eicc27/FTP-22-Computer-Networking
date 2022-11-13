#include "command.h"
#include <fcntl.h>

char msg[MAX_LEN];
// TODO: implement functions client_xxx

const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, HELP_GET, client_get},
    {SEM_PUT, 1, CMD_PUT, HELP_PUT, client_put},
    {SEM_DEL, 1, CMD_DEL, HELP_DEL, client_delete},
    {SEM_LS, 0, CMD_LS, HELP_LS, client_ls},
    {SEM_CD, 1, CMD_CD, HELP_CD, client_cd},
    {SEM_MKD, 1, CMD_MKD, HELP_MKD, client_mkdir},
    {SEM_PWD, 0, CMD_PWD, HELP_PWD, client_pwd},
    {SEM_QUIT, 0, CMD_QUIT, HELP_QUIT, client_quit}};

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
int client_get(int sockfd, const char *arg)
{
    // file buffer for storing the data server sent
    char *fileBuffer = (char *)calloc(MAX_LEN, sizeof(char));
    // local file
    int fileLocal;
    // bytes num read from server
    int nbytes;
    printf("client get %s\n", arg);
    // client `arg` ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
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
    return 0;
}

/*Run `put` command to put(upload) a given file to the server's given directory.
 */
int client_put(int sockfd, const char *arg)
{
    printf("client put %s\n", arg);
    // client `arg` ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    // open the new local file
    int fileLocal;
    if ((fileLocal = open(arg + 4, O_RDONLY)) < 0)
    {
        printf("Open error: unable to open local file\n");
        return -1;
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
    return 0;
}

/*Run `delete` to remove a given directory on the server.
The client just simply prints the value the server returns.
 */
int client_delete(int sockfd, const char *arg)
{

    // client arg ---> server
#ifdef _WIN32
    char delete_cmd[MAX_LEN] = { 0 };
    strcpy_s(delete_cmd, strlen(CMD_DEL) + 1, CMD_DEL);
    strcat_s(delete_cmd, strlen(CMD_DEL) + 1 + strlen(" "), " ");
    strcat_s(delete_cmd, strlen(CMD_DEL) + 1 + strlen(arg) + strlen(" "), arg);
    if (send(sockfd, (char*)delete_cmd, strlen(delete_cmd) + 1, 0) <= 0) {
        printf("Send error: Failed to send to the server.%d\n", WSAGetLastError());
        return 1;
    }

    if (recv(sockfd, (char*)msg, MAX_LEN, 0) > 0) {
        printf("%s", msg);
    }
    else {
        printf("Recv error: Failed to recv to the server%d.\n", WSAGetLastError());
    }
#else
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, arg, MAX_LEN) > 0)
        printf("%s", arg);
#endif

    return 0;
}

/*Run `ls` command to list the given directory on the server, which then returns the listed directory.
The client just simply prints the value the server returns.
TODO: This interface sends the argument directly to the server.
The server has to deal with and parse these different arguments.
*/
int client_ls(int sockfd, const char *arg)
{
    if (arg)
        printf(IGNORE_ARGUMENT(ls));
    
    // client `ls`---> server
#ifdef _WIN32
    if (send(sockfd, CMD_LS, strlen(CMD_LS)+1, 0) <= 0) {
        printf("Send error: Failed to send to the server%d\n", WSAGetLastError());
        return -1;
    }
    char fileNameBuffer[MAX_LEN] = {0};
    if(recv(sockfd, (char*)fileNameBuffer, MAX_LEN, 0)>0) {
        printf("%s\n", fileNameBuffer);
    }else {
        printf("Recv error: Failed to recv to the server%d\n", WSAGetLastError());
    }
#else
    if (write(sockfd, CMD_LS, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("list in dir...\n");
    // server `sockfd` ---> arg
    while (read(sockfd, arg, MAX_LEN) > 0)
        printf("%s", arg);
#endif


    return 0;
}

/*Run `cd` to change into a different server's directory.
The client just simply prints the value the server returns.
TODO: Like powershell on Windows, `cd` changes the 'prefix'
of the client interface. Just like:
C:/Program Files>cd ..
C:/>
*/
int client_cd(int sockfd, const char *arg)
{
    
    

#ifdef _WIN32
    char position[MAX_LEN] = { 0 };
    strcpy_s(position, strlen(CMD_CD) + 1, CMD_CD);
    strcat_s(position, strlen(CMD_CD) + 1 + strlen(" "), " ");
    strcat_s(position, strlen(CMD_CD) + 1 + strlen(arg) + strlen(" "),arg);
    if (send(sockfd, (char*)position, strlen(position) + 1, 0) <= 0) {
        printf("Send error: Failed to send to the server.%d\n", WSAGetLastError());
        return -1;
    }
    
    if (recv(sockfd, (char*)msg, MAX_LEN, 0) > 0) {
        
    }
    else {
        printf("Recv error: Failed to recv to the server%d.\n", WSAGetLastError());
    }
#else
    
    // client arg ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, arg, MAX_LEN) > 0)
        printf("%s", arg);
#endif

    return 0;
}

/*Run `mkdir` to make a new directory on client.
The client just simply prints the value the server returns.
*/
int client_mkdir(int sockfd, const char *arg)
{
#ifdef _WIN32
    char folderName[MAX_LEN] = { 0 };
    strcpy_s(folderName, strlen(CMD_MKD) + 1, CMD_MKD);
    strcat_s(folderName, strlen(CMD_MKD) + 1 + strlen(" "), " ");
    strcat_s(folderName, strlen(CMD_MKD) + 1 + strlen(arg) + strlen(" "), arg);
    if (send(sockfd, (char*)folderName, strlen(folderName) + 1, 0) <= 0) {
        printf("Send error: Failed to send to the server.%d\n", WSAGetLastError());
        return -1;
    }

    if (recv(sockfd, (char*)msg, MAX_LEN, 0) > 0) {
        printf("%s", msg);
    }
    else {
        printf("Recv error: Failed to recv to the server%d.\n", WSAGetLastError());
    }
#else
    // client arg ---> server
    if (write(sockfd, arg, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, arg, MAX_LEN) > 0)
        printf("%s", arg);
#endif
    return 0;
}

/* Run `pwd` to print the current working directory on server.
*/
int client_pwd(int sockfd, const char *arg)
{
    if (arg)
        printf(IGNORE_ARGUMENT(pwd));

#ifdef _WIN32
    if (send(sockfd, CMD_PWD, strlen(CMD_PWD) + 1, 0) <= 0) {
        printf("Send error: Failed to send to the server%d\n", WSAGetLastError());
        return -1;
    }
    char position[MAX_LEN] = { 0 };
    if (recv(sockfd, (char*)position, MAX_LEN, 0) > 0) {
        printf("%s", position);
    }
    else {
        printf("Recv error: Failed to recv to the server%d\n", WSAGetLastError());
    }
#else
    char buffer[MAXPATH];
    _getcwd(buffer, MAXPATH);
    // client arg ---> server
    if (write(sockfd, CMD_PWD, MAX_LEN) < 0)
    {
        printf("Write error: Failed to write to the server\n");
        exit(1);
    }
    printf("client delete... %s\n", arg);
    // server `sockfd` ---> arg
    while (read(sockfd, arg, MAX_LEN) > 0)
        printf("%s", arg);
    printf("client pwd\n");
#endif

    return 0;
}

int client_quit(int sockfd, const char *arg)
{ // arg not used
    if (arg)
        printf(IGNORE_ARGUMENT(quit));
    printf("leaving ftp...\n");
    return 1;
}

int client_err(int sockfd, const char *arg)
{
    printf("invalid command\n");
    help();
    return -1;
}

int client_null(int sockfd, const char *arg)
{
    return -1;
}

void help()
{
    for_i_in_range(CMD_NUM)
        printf("%8s -- %s\n", commands[i].cmd, commands[i].help);
}
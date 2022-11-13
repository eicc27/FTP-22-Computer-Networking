#include "command.h"
#include <fcntl.h>
char buffer[256];
char cwd[256];
// TODO: implement functions client_xxx

const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, HELP_GET, client_get},
    {SEM_PUT, 1, CMD_PUT, HELP_PUT, client_put},
    {SEM_DEL, 1, CMD_DEL, HELP_DEL, server_delete},
    {SEM_LS, 0, CMD_LS, HELP_LS, server_ls},
    {SEM_CD, 1, CMD_CD, HELP_CD, server_cd},
    {SEM_MKD, 1, CMD_MKD, HELP_MKD, server_mkdir},
    {SEM_PWD, 0, CMD_PWD, HELP_PWD, server_pwd},
    {SEM_QUIT, 0, CMD_QUIT, HELP_QUIT, client_quit}};

const Command error_command = {SEM_ERR, 0, NULL_STR, "", client_err};
const Command null_command = {SEM_ERR, 0, NULL_STR, "", client_null};


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
bool server_delete(SOCKET s, const char *path)
{
#ifdef _WIN32
    if (remove(path) == 0) {
        char msg[] = "文件删除成功\n";
        printf("%s",msg);
        if (send(s, (char*)msg, MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
    }
    else {
        char msg[] = "文件删除失败,文件夹禁止删除或文件不存在\n";
        printf("%s", msg);
        if (send(s, (char*)msg, MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
        return false;
    }
#else



#endif
    
    return true;
}

/*Run `ls` command to list the given directory on the server, which then returns the listed directory.
The client just simply prints the value the server returns.
TODO: This interface sends the argument directly to the server.
The server has to deal with and parse these different arguments.
*/
bool server_ls(SOCKET s,const char * path)
{

    #ifdef _WIN32

    struct _finddata_t file;
    intptr_t   hFile;
    hFile = _findfirst("*", &file);
    _findnext(hFile, &file);//把输出..(当前文件夹)去掉
    char fileNameBuf[100] = { 0 };
    memset(buffer,0,sizeof(buffer));
    while (_findnext(hFile, &file) == 0)
    {   
        struct stat buf;
        int result;
        result = stat(file.name, &buf);
        if (S_IFDIR & buf.st_mode) {
            strcpy_s(fileNameBuf, strlen(file.name)+1, file.name);
            strcat_s(fileNameBuf, strlen(file.name)+1 +strlen("/    "), "/    ");
        }
        else if (S_IFREG & buf.st_mode) {
            strcpy_s(fileNameBuf, strlen(file.name)+1, file.name);
            strcat_s(fileNameBuf, strlen(file.name)+1 + strlen("    "), "    ");
        }
        strcat_s(buffer, strlen(buffer) + strlen(fileNameBuf) + 1, fileNameBuf);
        
    }
    printf("%s\n",buffer);
    if (send(s, (char *)buffer, MAX_LEN,0)<=0) {
        printf("Send error: Failed to send to the client.%d\n",WSAGetLastError());
        return false;
    }
    #else
    


    #endif
    return true;
}

/*Run `cd` to change into a different server's directory.
The client just simply prints the value the server returns.
TODO: Like powershell on Windows, `cd` changes the 'prefix'
of the client interface. Just like:
C:/Program Files>cd ..
C:/>
*/
bool server_cd(SOCKET s, const char* path)
{
#ifdef _WIN32
    if (_chdir(path))
    {
        printf("打开文件夹失败.\n");
        char msg[] = "打开文件夹失败\n";
        strcpy_s(buffer, strlen(msg) + 1, msg);
        if (send(s, buffer, strlen(buffer), 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            exit(1);
        }
        return false;
    }
    _getcwd(cwd, sizeof(cwd) != 0);


    printf("cwd:%s\n",cwd);
    if (send(s, (char*)cwd, strlen(cwd)+1, 0) <= 0) {
        printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
        exit(1);
    }
    printf("%s", buffer);
#else


#endif
    return true;
}

/*Run `mkdir` to make a new directory on client.
The client just simply prints the value the server returns.
*/
bool server_mkdir(SOCKET s, const char * folderName)
{
#ifdef _WIN32
    if (_access(folderName, 0) == -1)
    {
        _mkdir(folderName);
        if (send(s, "文件夹创建成功\n", MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
    }
    else {
        if (send(s, "文件夹已存在\n", MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
    }
#else


#endif
    return true;
}

/* Run `pwd` to print the current working directory on server.
*/
bool server_pwd(SOCKET s, const char *path)
{
#ifdef _WIN32

    _getcwd(buffer, sizeof(buffer));
    printf("%s\n", buffer);
    if (send(s, (char*)buffer, MAX_LEN, 0) <= 0) {
        printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
        return false;
    }
    printf("已向客户端发送当前工作目录:%s\n",buffer);
#else
    
#endif
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
    for_i_in_range(CMD_NUM)
        printf("%8s -- %s\n", commands[i].cmd, commands[i].help);
}
#include "command.h"
#include <fcntl.h>
char buffer[256];
char cwd[256];
// TODO: implement functions client_xxx

const Command commands[CMD_NUM] = {
    {SEM_GET, 1, CMD_GET, HELP_GET, server_get},
    {SEM_PUT, 1, CMD_PUT, HELP_PUT, server_put},
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
bool server_get(SOCKET s, const char *filename)
{
    
    Packet packet = {0};
    strcpy_s(packet.fileInfo.fileName,strlen(filename)+1, filename);
    FILE* pread;
    //�ж��Ƿ��ܴ��ļ�
    errno_t err = fopen_s(&pread, packet.fileInfo.fileName, "rb");
    if (err != 0) {
        printf("�Ҳ���[%s]�ļ�...\n", packet.fileInfo.fileName);
        packet.msg = FAILDFIND;
        if (SOCKET_ERROR == send(s, (char*)&packet, sizeof(Packet), 0)) {
            printf("send faild %d\n", WSAGetLastError());
        }
        return false;
    }
    //��ȡ�ļ���С
    int fileSize = 0;
    fseek(pread, 0, SEEK_END);
    fileSize = ftell(pread);
    fseek(pread, 0, SEEK_SET);
    fclose(pread);
    packet.fileInfo.fileSize = fileSize;
    packet.msg = FILESIZE;
    //��֪�ͻ����ļ���С�Ա�����Ƿ�����
    if (SOCKET_ERROR == send(s, (char*)&packet, sizeof(Packet), 0)) {
        printf("send faild %d\n", WSAGetLastError());
    }
    //�ͻ��˷����Ƿ�����
    if (recv(s, (char*)&packet, sizeof(Packet), 0)<= 0) {
        printf("Recv error: Failed to recv to the server%d", WSAGetLastError());
    }
    switch (packet.msg) {
        case CANCEL:
            printf("�ͻ���ȡ��������.\n");
            break;
        case READYDOWN:
            printf("�������ص��ͻ���...\n");
            uploadToC(s,&packet);
            break;
        default:
            printf("Unknown message.");
            break;

    }
    
        
    
    return true;
}

/*Run `put` command to put(upload) a given file to the server's given directory.
 */
bool server_put(SOCKET s, const char* filename)
{
    Packet packet = { 0 };
    strcpy_s(packet.fileInfo.fileName, strlen(filename) + 1, filename);
    packet.msg = FILENAME;
    printf("filename:%s\n", packet.fileInfo.fileName);
    if (SOCKET_ERROR == send(s, (char*)&packet, sizeof(Packet), 0)) {
        printf("send faild %d\n", WSAGetLastError());
    }
    if (recv(s, (char*)&packet, sizeof(Packet), 0) <= 0) {
        printf("Recv error: Failed to recv to the server%d", WSAGetLastError());
    }
    

    printf("fileSize:%d\n", packet.fileInfo.fileSize);
    printf("�ռ��㹻\n");
    packet.msg = READYDOWN;
    if (send(s, (char*)&packet, sizeof(Packet), 0)<=0) {
        printf("send faild %d\n", WSAGetLastError());
    }
    
    if (download(s, &packet) == 0) {
            printf("�������.");
    };
    return true;
}

/*Run `delete` to remove a given directory on the server.
The client just simply prints the value the server returns.
 */
bool server_delete(SOCKET s, const char *path)
{
#ifdef _WIN32
    if (remove(path) == 0) {
        char msg[] = "�ļ�ɾ���ɹ�";
        printf("%s",msg);
        if (send(s, (char*)msg, MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
    }
    else {
        char msg[] = "�ļ�ɾ��ʧ��,�ļ��н�ֹɾ�����ļ�������";
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
    _findnext(hFile, &file);//�����..(��ǰ�ļ���)ȥ��
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
    printf("%s",buffer);
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
        printf("���ļ���ʧ��.\n");
        char msg[] = "���ļ���ʧ��\n";
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
        if (send(s, "�ļ��д����ɹ�", MAX_LEN, 0) <= 0) {
            printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
            return false;
        }
    }
    else {
        if (send(s, "�ļ����Ѵ���", MAX_LEN, 0) <= 0) {
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
    printf("����ͻ��˷��͵�ǰ����Ŀ¼:%s\n",buffer);
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

int download(SOCKET s, Packet* packet) {
    Packet recvPacket = { 0 };
    FILE* pwrite;
    char fileName[50] = { 0 };
    strcpy_s(fileName, strlen(packet->fileInfo.fileName) + 1, packet->fileInfo.fileName);
    int fileSize = packet->fileInfo.fileSize;
    char* fileBuf = calloc(sizeof(packet->dataInfo.dataBuf), sizeof(char));
    if (fileBuf == NULL) {
        printf("�ռ�����ʧ��\n");
        return -1;
    }
    //�ж��Ƿ��ܴ��ļ�
    printf("fileName:%s\n", packet->fileInfo.fileName);
    errno_t err = fopen_s(&pwrite, fileName, "wb");
    if (pwrite == NULL) {
        printf("write file error..\n");
        return -1;
    }
    //���մӷ����������İ�
    printf("��������...\n");
    long speed = 0;
    char pace[102] = { 0 };
    while (recvPacket.dataInfo.offset <= fileSize) {
        if (recv(s, (char*)&recvPacket, sizeof(Packet), 0) <= 0) {
            printf("send error: %d", WSAGetLastError());
            fclose(pwrite);
            return -1;
        }
        //���������ݸ��Ƶ�����
        memcpy(fileBuf, recvPacket.dataInfo.dataBuf, recvPacket.dataInfo.dataBufSize);

        //����������д���ļ�
        fwrite(fileBuf, sizeof(char), recvPacket.dataInfo.dataBufSize, pwrite);

        int _speed = fileSize < 100 ? 100 : recvPacket.dataInfo.offset / (fileSize / 100);
        if (speed != _speed) {
            speed = _speed;
            for (int j = 0; j < speed; j++) {
                pace[j] = '>';
            }
            printf("[%-100s][%d%%]\r", pace, speed);
            fflush(stdout);




        }
        //ȫ��������ɺ��˳�
        if (recvPacket.dataInfo.offset == fileSize) {
            break;
        }




    }
    printf("\n");

    fclose(pwrite);

    free(fileBuf);

    return 0;
}

int uploadToC(SOCKET s,Packet * packet) {
    FILE* pread;
    //�ж��Ƿ��ܴ��ļ�
    errno_t err = fopen_s(&pread, packet->fileInfo.fileName, "rb");
    int fileSize = packet->fileInfo.fileSize;
    Packet sendPacket = {0};
    int bufSize = sizeof(sendPacket.dataInfo.dataBuf);
    char dataBuf[sizeof(sendPacket.dataInfo.dataBuf)] = {0};
    //printf("sendSize%d\n", packet->fileInfo.fileSize);
    sendPacket.msg = DOWNLOADING;
    //���ļ���С����bufʱ����������
    int i = 0;
    if (bufSize < fileSize) {
        sendPacket.dataInfo.dataBufSize = bufSize;
        for (i = 0; i < fileSize; i += bufSize) {
            sendPacket.dataInfo.offset = i;

            fread(&dataBuf,sizeof(char),bufSize, pread);
            
            memcpy(sendPacket.dataInfo.dataBuf, dataBuf,bufSize);
            if (send(s, (char*)&sendPacket, MAX_LEN, 0) <= 0) {
                printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
                fclose(pread);
                return -1;
            }
        }
        i = i - bufSize;
    }
    int lastBufSize = fileSize - i;
    printf("bufsize:%d\n", lastBufSize);
    sendPacket.dataInfo.dataBufSize = lastBufSize;
    fread(&dataBuf, sizeof(char), lastBufSize, pread);
    memcpy(sendPacket.dataInfo.dataBuf,  dataBuf,sizeof(dataBuf));
    sendPacket.dataInfo.offset += lastBufSize;
    if (send(s, (char*)&sendPacket, MAX_LEN, 0) <= 0) {
        printf("Send error: Failed to send to the client.%d\n", WSAGetLastError());
        fclose(pread);
        return -1;
    }
    fclose(pread);
    return 0;



}

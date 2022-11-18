#ifndef __CLIENT_UTILS
#define __CLIENT_UTILS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#ifdef __linux__
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#include <process.h>
#include <WinSock2.h>
#define F_OK 00
#define access(A, B) _access(A, B)
typedef struct sockaddr_in in_addr_t;
#endif

#include "../common/language_server.h"

extern char *ip;
extern unsigned long port;

typedef struct Connection {
    in_addr_t ip_addr;
    int pid;
    char cmd[MAX_LEN];
    char buf[MAX_LEN];
} Connection;

void init_connection(struct sockaddr_in *);
void exit_handler(int);
char *get_pwd(struct sockaddr_in *);
void pwd_handler(int *, struct sockaddr_in *, char *);
void ls_handler(int *, struct sockaddr_in *, char *);
void cd_handler(int *, struct sockaddr_in *, char *);
void mkdir_handler(int *, struct sockaddr_in *, char *);
void remove_handler(int *, struct sockaddr_in *, char *);
void get_handler(int *, struct sockaddr_in *, char *);
void put_handler(int *, struct sockaddr_in *, char *);
void quit_handler(int *, struct sockaddr_in *, char *);
void unknown_command_handler(char *command);
#endif
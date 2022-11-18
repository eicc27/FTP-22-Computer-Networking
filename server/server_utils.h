#ifndef __SERVER_UTILS
#define __SERVER_UTILS

#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/language_server.h"

#define SEVR_VER "0.1"
#define MAX_CONN 5
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

typedef struct Connection {
  in_addr_t ip_addr;
  pid_t pid;
  char cmd[MAX_LEN];
  char buf[MAX_LEN];
  struct Connection *next_conn;
} Connection;

void print_init_info();
void print_ip_in_str(in_addr_t);

Connection *init_connections(Connection *);
void insert_connection(Connection **, Connection, char *);
void delete_connection(Connection **, in_addr_t, pid_t);
int count_connection(Connection **);

void init_handler(int *, int *, Connection **, Connection *, char *);
void quit_handler(int *, int *, Connection **, Connection *);
void pwd_handler(int *, int *, Connection **, Connection *);
void ls_handler(int *, int *, Connection **, Connection *);
void cd_handler(int *, int *, Connection **, Connection *);
void mkdir_handler(int *, int *, Connection **, Connection *);
void remove_handler(int *, int *, Connection **, Connection *);
void get_handler(int *, int *, Connection **, Connection *);
void get_cont_handler(int *, int *, Connection **, Connection *);
void put_handler(int *, int *, Connection **, Connection *);
void put_cont_handler(int *, int *, Connection **, Connection *);


void command_get(int, char *);
void command_put(int, char *);

#endif
# ifndef __UTIL__
# define __UTIL__

// TODO: use socket in windows / linux

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <strings.h>
# include <stdbool.h>
# include <errno.h>

# ifdef _WIN32

# include <winsock2.h>
# include <io.h>
#define bzero(a, b) memset(a, 0, b) // Fixed: for Windows use general memset function

# elif __linux__

# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <fcntl.h>


# include <dirent.h> // for ls

// end of `DEFINE _WIN32`
# endif


# define MAX_ARGS 16
# define MAX_LEN 256

# define for_i_in_range(r) for(int i=0;i<(r);i++)

typedef struct{
    short argc;
    const char* argv[MAX_ARGS];
    const char* comment;
    bool err;
} Arguments;


typedef struct sockaddr_in sockaddr_in;

void clear_buffer();
Arguments split_string(const char*);
int new_socket_conn(sockaddr_in);
int write_socket_conn(); // TODO
int read_socket_conn(); // TODO
void close_socket_conn(int); // TODO

char *unshift_s(char, char *);
char *unshift_a(const char *, char *);

// end of `#define __UTIL__`
# endif
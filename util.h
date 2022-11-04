# ifndef __UTIL__
# define __UTIL__

// TODO: use socket in windows / linux

# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <errno.h>

# ifdef _WIN32

# include <winsock2.h>
# include <io.h>


# elif __linux__

# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>


# include <dirent.h> // for ls

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

void clear_buffer();
Arguments split_string(const char*);

# endif
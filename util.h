# ifndef __UTIL__
# define __UTIL__

// TODO: use socket in windows / linux
// TODO: more common tools

# include <stdio.h>
# include <string.h>
# include <stdbool.h>

# define MAX_ARGS 16

# define for_i_in_range(r) for(int i=0;i<(r);i++)

typedef struct{
    short argc;
    const char* argv[MAX_ARGS];
    const char* comment;
    bool err;
} Arguments;

void clear_buffer();
Arguments split_input(const char*);

# endif
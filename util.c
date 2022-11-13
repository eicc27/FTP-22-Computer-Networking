#include "util.h"
#include <stdlib.h>
#include <string.h>

void clear_buffer()
{
    while (getchar() != '\n')
        ;
}

Arguments split_string(const char *input)
{
    const char *p = NULL;
    Arguments args = {0, 0};

    if (!input || !strlen(input))
    {
        args.comment = "input is null!";
        printf("input is null!");
        args.err = true;
        return args;
    }

    p = strtok((char *)input, " ");
    args.argv[0] = p;
    args.argc++;
    while ((p = strtok(NULL, " ")) != NULL)
    {
        args.argv[args.argc] = p;
        args.argc++;
    }

    return args;
}

/* Initializes a new client socket connection. If any error
happens in socket initialization or connection, quit the client
with ret val 1.
*/
int new_socket_conn(sockaddr_in addr)
{
    // under Windows
    int sockfd;
    // # ifdef _WIN32
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    // #endif
    return sockfd;
}

void close_socket_conn(int sockfd)
{
    // under Windows
    close(sockfd);
}

/* Add a single character in front of the whole array.
*/
char *unshift_s(char c, char *arr)
{
    const int len = strlen(arr);
    arr = (char*) realloc(arr, (len + 1) * sizeof(char));
    for (int i = len; i >= 1; i--) {
        arr[i] = arr[i - 1];
    }
    arr[0] = c;
    return arr;
}

/* Add an array in front of a given array.
*/
char* unshift_a(const char* s, char *arr) {
    const int slen = strlen(s);
    for (int i = slen - 1; i >= 0; i--) {
        arr = unshift_s(s[i], arr);
    }
    return arr;
}

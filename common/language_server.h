#ifndef __LANGUAGE_SERVER
#define __LANGUAGE_SERVER
#include "colored_string.h"
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>

#define MAX_CMD_LEN 32
#define BKSPACE 127

extern const char COMMANDS[][MAX_CMD_LEN];

typedef struct pbar_t {
    long comment_len;
    int pbar_len;
    int total;
    int current;
    pthread_t *listener;
} pbar_t;

pbar_t *init_pbar(int total, char *comment);
void update_pbar(pbar_t *pbar, int amount);
void close_pbar(pbar_t *pbar);

int getKbd();

void open_term_buf();
void close_term_buf();

void cursor_up(int n);
void cursor_down(int n);
void cursor_left(int n);
void cursor_right(int n);

void save_term_cursor();
void load_term_cursor();

void clear_to_line_end();
void clear();

char **split_with_space(char *s);
int in_commands(char *string);
int len_of_strings(char **strings);

int isdir(char *cwd, char *dir);
int _isdir(char *dir);
int file_exists(char *cwd, char *file);
char* file_name(char* dir);
int isnum(char c);
int is_ip(char *ip);
void free_words(char **words);
#endif
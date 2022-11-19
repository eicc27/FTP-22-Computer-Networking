#include "language_server.h"
#include "colored_string.h"
#include <stdlib.h>
#include <sys/stat.h>
#ifdef __linux__
#include <bits/pthreadtypes.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#else
#include <direct.h>
#include <processthreadsapi.h>
typedef HANDLE pthread_t;
#endif

const char COMMANDS[][MAX_CMD_LEN] = {"cd",     "pwd", "ls",  "mkdir",
                                      "delete", "get", "put", "quit"};

void open_term_buf() { system("stty icanon"); }

void close_term_buf() { system("stty -icanon"); }

void save_term_cursor() { printf("\033[s"); }
void load_term_cursor() { printf("\033[u"); }

void cursor_right(int n) { printf("\033[%dC", n); }
void cursor_left(int n) { printf("\033[%dD", n); }
void cursor_up(int n) { printf("\033[%dA", n); }
void cursor_down(int n) { printf("\033[%dB", n); }

void clear_to_line_end() { printf("\033[K"); }
void clear() { printf("\033[2J"); }

char **split_with_space(char *s) {
  long l = strlen(s);
  if (s[l - 1] == '\n') // for input cases
    l--;
  char **res = calloc(32, sizeof(char *));
  int j = 0, k = 0;
  for (int i = 0; i < l; i++) {
    if (i == 0 || (i < l - 1 && s[i] == ' ' && s[i + 1] != ' ')) {
      k = 0;
      res[j++] = calloc(MAX_LEN, sizeof(char));
    }
    if (s[i] != ' ')
      res[j - 1][k++] = s[i];
  }
  return res;
}

int len_of_strings(char **strings) {
  int i = 0;
  while (strings[i++] != NULL)
    ;
  return i - 1;
}

int in_commands(char *string) {
  for (int i = 0; i < 8; i++) {
    if (strcmp(COMMANDS[i], string) == 0)
      return i;
  }
  return -1;
}

void *listen_to_pbar(pbar_t *pbar) {
  while (pbar->current < pbar->total) {
    int progress = (10 * pbar->current) / pbar->total;
    int percentage = (100 * pbar->current) / pbar->total;
    char percentage_str[4] = {'\0'};
    sprintf(percentage_str, "%d%%", percentage);
    // set cursor back to progress bar
    int base = pbar->pbar_len;
    cursor_left(base);
    base = pbar->comment_len + 3;
    cursor_right(base);
    for (int i = 0; i < progress; i++)
      printf("=");
    cursor_right(11 - progress);
    pbar->pbar_len =
        pbar->comment_len + strlen(": [          ] ") + strlen(percentage_str);
    clear_to_line_end();
    printf("%d%%", percentage);
    fflush(stdout);
    sleep(1);
  }
  return NULL;
}

pbar_t *init_pbar(int total, char *comment) {
  pbar_t *pbar = calloc(1, sizeof(pbar_t));
  pbar->current = 0;
  pbar->total = total;
  pbar->comment_len = strlen(comment);
  pbar->pbar_len = pbar->comment_len + strlen(": [          ] 0%%");
  printf("%s: [          ] 0%%", comment);
  fflush(stdout);
  pbar->listener = calloc(1, sizeof(pthread_t));
  pthread_create(pbar->listener, NULL, (void *(*)(void *))listen_to_pbar, pbar);
  return pbar;
}

void update_pbar(pbar_t *pbar, int amount) { pbar->current += amount; }

void close_pbar(pbar_t *pbar) {
  pthread_join(*pbar->listener, NULL);
  cursor_left(pbar->pbar_len);
  printf("Done.");
  fflush(stdout);
  clear_to_line_end();
  printf("\n");
  if (pbar->total < pbar->current)
    printf("%s\n", yellow("Progress bar overflow!"));
}

char *file_name(char *dir) {
  long l = strlen(dir);
  char *filename = calloc(MAX_LEN, sizeof(char));
  long i = l - 1;
  for (; i >= 0; i--) {
    if (dir[i] == '/' || dir[i] == '\\')
      break;
  }
  long flen = l - i - 1;
  for (int j = 0; j < flen; j++) {
    filename[j] = dir[j + i + 1];
  }
  return filename;
}
#ifdef __linux
int isdir(char *cwd, char *dir) {
  DIR *d = opendir(cwd);
  struct dirent *ent = NULL;
  while ((ent = readdir(d))) {
    if (ent->d_type == DT_DIR)
      if (strcmp(ent->d_name, dir) == 0)
        return 1;
  }
  return 0;
}

int _isdir(char *dir) {
  struct stat s;
  int result = stat(dir, &s);
  if (result != 0)
    return 0;
  if (s.st_mode & S_IFDIR)
    return 1;
  return 0;
}

int file_exists(char *cwd, char *file) {
  DIR *d = opendir(cwd);
  struct dirent *ent = NULL;
  while ((ent = readdir(d))) {
    if (ent->d_type == DT_REG)
      if (strcmp(ent->d_name, file) == 0)
        return 1;
  }
  return 0;
}

char **split_with_dot(char *s) {
  long l = strlen(s);
  if (s[l - 1] == '\n') // for input cases
    l--;
  char **res = calloc(MAX_LEN, sizeof(char *));
  int j = 0, k = 0;
  for (int i = 0; i < l; i++) {
    if (j >= 5)
      return res;
    if (i == 0 || (i < l - 1 && s[i] == '.' && s[i + 1] != '.')) {
      k = 0;
      res[j++] = calloc(MAX_LEN, sizeof(char));
    }
    if (s[i] != '.' && k < 4)
      res[j - 1][k++] = s[i];
  }
  return res;
}

int isnum(char c) { return c >= 48 && c <= 57; }

int is_purenum(char *s) {
  long l = strlen(s);
  for (int i = 0; i < l; i++) {
    if (!isnum(s[i]))
      return 0;
  }
  return 1;
}

int is_ip(char *ip) {
  if (!isnum(ip[strlen(ip) - 1])) {
    return 0;
  }
  char **words = split_with_dot(ip);
  int l = len_of_strings(words);
  if (l != 4) {
    free_words(words);
    return 0;
  }
  for (int i = 0; i < l; i++) {
    if (!is_purenum(words[i]) || atoi(words[i]) > 255) {
      free_words(words);
      return 0;
    }
  }
  free_words(words);
  return 1;
}

void free_words(char **words) {
  int l = len_of_strings(words);
  for (int i = 0; i < l; i++)
    free(words[i]);
  free(words);
}
#else
int isdir(char *cwd, char *dir) {}

#endif

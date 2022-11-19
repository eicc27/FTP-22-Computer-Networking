#include "client_utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ip = NULL;
unsigned long port = 0;

int is_command_available(const char *cmd, char *command, int num_words,
                         int *is_command) {
  char **words = split_with_space(command);
  if (len_of_strings(words) == 0) {
    printf("%s\n", red("Input is empty!"));
    *is_command = 1;
    free_words(words);
    return 0;
  }
  if (strcmp(words[0], cmd) != 0) {
    free_words(words);
    return 0;
  }
  int ret_val = 1;
  if (len_of_strings(words) != num_words) {
    ret_val = 0;
    if (strcmp(words[0], cmd) == 0) {
      printf("%s: %s\n", yellow((char *)cmd),
             red("Command recieved invalid parameters."));
      *is_command = 1;
    }
    free_words(words);
    return ret_val;
  }
  free_words(words);
  return 1;
}

int init_sockfd(struct sockaddr_in addr) {
  int sockfd;
  // 创建套接字
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("%s\n", red("Socket failed to init."));
    exit(1);
  }

  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("%s\n", red("Connection with server lost."));
    exit(1);
  }
  return sockfd;
}

void init_connection(struct sockaddr_in *addr) {
  int sockfd = init_sockfd(*addr);
  Connection conn = {
      .ip_addr = addr->sin_addr.s_addr,
      .pid = getpid(),
      .cmd = "init",
  };
  if (write(sockfd, &conn, sizeof(Connection)) < 0) {
    printf("Write Error!\n");
    exit(1);
  }
  char *msg = calloc(13, sizeof(char));
  read(sockfd, msg, 13);
  printf("%s\n", msg);
  close(sockfd);
  return;
}

void exit_handler(int sig) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  int sockfd = init_sockfd(addr);
  Connection conn = {
      .cmd = "quit", .ip_addr = addr.sin_addr.s_addr, .pid = getpid()};
  if (write(sockfd, &conn, sizeof(Connection)) < 0) {
    printf(
        "%s\n",
        red("Exit error: Client could not connect to server! The client will "
            "directly exit..."));
    close(sockfd);
    exit(1);
  }
  printf("\n%s\n", yellow("Closing FTP Client..."));
  char msg[14] = {'\0'};
  read(sockfd, msg, 14);
  printf("%s\n", blue(msg));
  close(sockfd);
  printf("%s\n", green("Goodbye."));
  exit(0);
}

Connection set_connection(struct sockaddr_in *addr, char *command) {
  Connection c = {.ip_addr = addr->sin_addr.s_addr, .pid = getpid()};
  strcpy(c.cmd, command);
  return c;
}

char *get_pwd(struct sockaddr_in *addr) {
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, "pwd");
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  return response;
}

void pwd_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("pwd", command, 1, is_command))
    return;
  printf("%s\n", blue("Current working remote directory:"));
  *is_command = 1;
  char *response = get_pwd(addr);
  printf("%s\n", colored(response, ATTR_UNDRL, NULL, NULL));
  free(response);
}

void ls_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("ls", command, 1, is_command))
    return;
  printf("LS:\n");
  *is_command = 1;
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, "ls");
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  printf("%s\n", response);
  free(response);
}

void cd_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("cd", command, 2, is_command))
    return;
  *is_command = 1;
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, command);
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  // printf("CD:\n");
  printf("%s\n", response);
  free(response);
}

void mkdir_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("mkdir", command, 2, is_command))
    return;
  *is_command = 1;
  if (strlen(command) < 6) {
    printf("%s\n", red("mkdir must have a target. Operation aborted.\n"));
    return;
  }
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, command);
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  printf("MKDIR:\n");
  printf("%s\n", response);
  free(response);
}

void remove_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("delete", command, 2, is_command))
    return;
  *is_command = 1;
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, command);
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  printf("DELETE:\n");
  printf("%s\n", response);
  free(response);
}

// HEADER get + CONTENT get
void get_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  // HEADER get
  if (*is_command)
    return;
  if (!is_command_available("get", command, 2, is_command))
    return;
  *is_command = 1;
  int sockfd = init_sockfd(*addr);
  Connection c = set_connection(addr, command);
  if (write(sockfd, &c, sizeof(Connection)) < 0) {
    printf("%s\n",
           red("Command failed! The server might be down. Now exiting..."));
    exit(1);
  }
  char *response = calloc(MAX_LEN, sizeof(char));
  read(sockfd, response, MAX_LEN);
  close(sockfd);
  if (strcmp(response, "NaN") == 0) {
    printf("%s: %s", yellow("get"), red("File does not exist\n"));
    return;
  }
  long content_length = 0;
  sscanf(response, "%ld", &content_length);
  long get_num = content_length % MAX_LEN == 0 ? content_length / MAX_LEN
                                               : content_length / MAX_LEN + 1;
  printf("GET:\n");
  printf("Content-Length: %ld\n", content_length);
  // CONTENT get
  // use resp temporaily to store the directory specified
  memset(response, 0, MAX_LEN);
  strncpy(response, command + 4, strlen(command) - 4);
  FILE *f = fopen(file_name(response), "wb+");
  if (f == NULL) {
    response = yellow(command);
    strcat(response, red(": Write file failed. Check file name.\n"));
    return;
  }
  char *buf = calloc(MAX_LEN, sizeof(char));
  pbar_t *pbar = init_pbar(get_num, "Download progress");
  for (long i = 0; i < get_num; i++) {
    int sockfd = init_sockfd(*addr);
    sprintf(c.cmd, "sget %ld %s", i, response);
    write(sockfd, &c, sizeof(Connection));
    memset(buf, 0, MAX_LEN * sizeof(char));
    read(sockfd, buf, MAX_LEN);
    fseek(f, i * MAX_LEN, SEEK_SET);
    if (i == get_num - 1) {
      if (content_length % MAX_LEN != 0) {
        fwrite(buf, content_length % MAX_LEN, 1, f);
      } else {
        fwrite(buf, MAX_LEN, 1, f);
      }
    } else
      fwrite(buf, MAX_LEN, 1, f);
    update_pbar(pbar, 1);
    close(sockfd);
  }
  close_pbar(pbar);
  free(buf);
  fclose(f);
  free(response);
}

// HEADER put + CONTENT put
void put_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  // HEADER put
  if (*is_command)
    return;
  if (!is_command_available("put", command, 2, is_command))
    return;
  *is_command = 1;
  char *buf = calloc(MAX_LEN, sizeof(char));
  char *fname = calloc(MAX_LEN, sizeof(char));
  sscanf(command, "put %s", fname);
  if (access(fname, F_OK) != 0) {
    printf("%s: %s\n", yellow("put"), red("File does not exist"));
    return;
  }
  FILE *f = fopen(fname, "rb");
  // first read file length
  fseek(f, 0, SEEK_END);
  long content_length = ftell(f);
  printf("Content-Length: %ld\n", content_length);
  fclose(f);
  int sockfd = init_sockfd(*addr);
  sprintf(buf, "put %ld %s", content_length, fname);
  Connection c = set_connection(addr, buf);
  write(sockfd, &c, sizeof(Connection));
  memset(buf, 0, MAX_LEN * sizeof(char));
  read(sockfd, buf, MAX_LEN);
  printf("%s", buf);
  close(sockfd);
  // CONTENT put
  long put_num = content_length % MAX_LEN == 0 ? content_length / MAX_LEN
                                               : content_length / MAX_LEN + 1;
  printf("Put num: %ld\n", put_num);
  FILE *fr = fopen(fname, "rb");
  pbar_t *pbar = init_pbar(put_num, "Upload progress");
  for (long i = 0; i < put_num; i++) {
    FILE *t = fopen("test.o", "ab+");
    int sockfd = init_sockfd(*addr);
    // first construct a sput command
    memset(buf, 0, MAX_LEN * sizeof(char));
    sprintf(buf, "sput %ld %ld %ld %s", i, put_num, content_length, fname);
    Connection c = set_connection(addr, buf);
    // then read the chunk...
    memset(buf, 0, MAX_LEN * sizeof(char));
    fseek(f, i * MAX_LEN, SEEK_SET);
    int sz =
        i == put_num - 1 ? content_length % MAX_LEN : MAX_LEN * sizeof(char);
    fread(c.buf, sz, 1, f);
    fwrite(c.buf, sz, 1, t);
    // ...and set the connection up
    // printf("Upload: %ld\n", strlen(c.buf));
    write(sockfd, &c, sizeof(Connection));
    memset(buf, 0, MAX_LEN * sizeof(char));
    read(sockfd, buf, MAX_LEN);
    // printf("Uploaded %s...\n", buf);
    update_pbar(pbar, 1);
    close(sockfd);
    fclose(t);
  }
  close_pbar(pbar);
  fclose(fr);
  free(fname);
  free(buf);
}

void quit_handler(int *is_command, struct sockaddr_in *addr, char *command) {
  if (*is_command)
    return;
  if (!is_command_available("quit", command, 1, is_command))
    return;
  exit_handler(0);
}

void unknown_command_handler(char *command) {
  char **s = split_with_space(command);
  if (len_of_strings(s) == 0) {
    free_words(s);
    return;
  }
  if (in_commands(s[0]) == -1) {
    printf("%s", red("Unknown command: "));
    printf("%s\n", red(command));
    free_words(s);
  }
}
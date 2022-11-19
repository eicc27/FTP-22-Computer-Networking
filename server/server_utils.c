#include "server_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_ip_in_str(in_addr_t ip) {
  __u_char *val = (__u_char *)&ip;
  printf("%u.%u.%u.%u", val[0], val[1], val[2], val[3]);
}

Connection *init_connections(Connection *conn0) {
  conn0->next_conn = NULL;
  return conn0;
}

void insert_connection(Connection **cs, Connection c, char *cwd) {
  Connection *f = *cs;
  while (f->next_conn != NULL) {
    f = f->next_conn;
  }
  printf("Before insertion, last conn's pid is %d\n", f->pid);
  Connection *tail = calloc(1, sizeof(Connection));
  getcwd(tail->cmd, MAX_LEN);
  tail->ip_addr = c.ip_addr;
  tail->pid = c.pid;
  tail->next_conn = NULL;
  f->next_conn = tail;
  printf("After insertion, last conn's pid is %d\n", c.pid);
}

void delete_connection(Connection **cs, in_addr_t ip, pid_t pid) {
  Connection *f = *cs;
  while (f != NULL) {
    Connection *fnext = f->next_conn;
    if (fnext->ip_addr == ip && fnext->pid == pid) {
      f->next_conn = fnext->next_conn;
      free(fnext);
      break;
    }
    f = f->next_conn;
  }
}

Connection *get_connection(Connection **cs, in_addr_t ip, pid_t pid) {
  Connection *f = *cs;
  while (f != NULL) {
    Connection *fnext = f->next_conn;
    if (fnext->ip_addr == ip && fnext->pid == pid)
      return fnext;
    f = f->next_conn;
  }
  return NULL;
}

int count_connection(Connection **cs) {
  Connection *f = *cs;
  int i = 0;
  while (f != NULL) {
    i++;
    f = f->next_conn;
  }
  return i - 1;
}

void print_init_info() {
  printf("FTP Server Version %s\n", SEVR_VER);
  printf("Successfully started\n");
}

void init_handler(int *is_command, int *cli_sockfd, Connection **connections,
                  Connection *command, char *cwd) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "init", 4) == 0) {
    *is_command = 1;
    write(*cli_sockfd, "Init Accepted", 13);
    insert_connection(connections, *command, cwd);
    printf("Current connections: %d\n", count_connection(connections));
  }
}

void quit_handler(int *is_command, int *cli_sockfd, Connection **connections,
                  Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "quit", 4) == 0) {
    *is_command = 1;
    delete_connection(connections, command->ip_addr, command->pid);
    write(*cli_sockfd, "Quit Accepted", 14);
    printf("-->quit\n");
    printf("Client IP ");
    print_ip_in_str(command->ip_addr);
    printf(", PID %d quit\n", command->pid);
    printf("Current connecions: %d\n", count_connection(connections));
  }
}

void pwd_handler(int *is_command, int *cli_sockfd, Connection **connections,
                 Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "pwd", 3) == 0) {
    *is_command = 1;
    char *response = calloc(MAX_LEN, sizeof(char));
    response = get_connection(connections, command->ip_addr, command->pid)->cmd;
    write(*cli_sockfd, response, MAX_LEN);
    // free(response);
  }
}

void ls_handler(int *is_command, int *cli_sockfd, Connection **connections,
                Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "ls", 2) == 0) {
    *is_command = 1;
    char *response = calloc(MAX_LEN, sizeof(char));
    DIR *dir = opendir(
        get_connection(connections, command->ip_addr, command->pid)->cmd);
    struct dirent *ent = NULL;
    while (1) {
      ent = readdir(dir);
      if (ent == NULL)
        break;
      if (ent->d_type == DT_REG) {   // Regular file
        if (ent->d_name[0] == '.') { // hidden file
          strcat(response, gray(ent->d_name));
          strcat(response, "\t");
          continue;
        }
        strcat(response, ent->d_name);
        strcat(response, "\t");
      } else if (ent->d_type == DT_DIR) {
        strcat(response, blue(ent->d_name));
        strcat(response, "\t");
      }
    }
    closedir(dir);
    write(*cli_sockfd, response, MAX_LEN);
    free(response);
  }
}

void cd_handler(int *is_command, int *cli_sockfd, Connection **connections,
                Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "cd", 2) == 0) {
    *is_command = 1;
    char *response = calloc(MAX_LEN, sizeof(char));
    // use resp temporaily to store the directory specified
    strncpy(response, command->cmd + 3, strlen(command->cmd) - 3);
    // first switch to the specified client's dir
    Connection *conn =
        get_connection(connections, command->ip_addr, command->pid);
    char *pwd = conn->cmd;
    chdir(pwd);
    printf("pwd: %s, target: %s\n", pwd, response);
    if (!_isdir( response)) {
      printf("Not a dir: %s\n", response);
      response = yellow(command->cmd);
      strcat(response, red(": Directory does not exist"));
      write(*cli_sockfd, response, MAX_LEN);
    } else {
      chdir(response);
      getcwd(conn->cmd, MAX_LEN);
      write(*cli_sockfd, response, MAX_LEN);
    }
    free(response);
  }
}

// todo: string check
void mkdir_handler(int *is_command, int *cli_sockfd, Connection **connections,
                   Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "mkdir", 5) == 0) {
    *is_command = 1;
    // use resp temporaily to store the directory specified
    char *response = calloc(MAX_LEN, sizeof(char));
    strncpy(response, command->cmd + 6, strlen(command->cmd) - 6);
    char *pwd =
        get_connection(connections, command->ip_addr, command->pid)->cmd;
    chdir(pwd);
    if (isdir(pwd, response)) {
      response = yellow(command->cmd);
      strcat(response,
             red(": Make directory failed: Directory already exists.\n"));
      write(*cli_sockfd, response, MAX_LEN);
    } else {
      mkdir(response, 0777);
      write(*cli_sockfd, green("success\n"), MAX_LEN);
    }
    free(response);
  }
}

// todo: file existence check
void remove_handler(int *is_command, int *cli_sockfd, Connection **connections,
                    Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "delete", 6) == 0) {
    *is_command = 1;
    // use resp temporaily to store the directory specified
    char *response = calloc(MAX_LEN, sizeof(char));
    strncpy(response, command->cmd + 7, strlen(command->cmd) - 7);
    chdir(get_connection(connections, command->ip_addr, command->pid)->cmd);
    if (remove(response) != 0) {
      response = yellow(command->cmd);
      strcat(response, red(": Remove file failed. Check file name.\n"));
    }
    write(*cli_sockfd, green("success\n"), MAX_LEN);
    free(response);
  }
}

void get_handler(int *is_command, int *cli_sockfd, Connection **connections,
                 Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "get", 3) == 0) {
    *is_command = 1;
    // use resp temporaily to store the directory specified
    char *response = calloc(MAX_LEN, sizeof(char));
    strncpy(response, command->cmd + 4, strlen(command->cmd) - 4);
    chdir(get_connection(connections, command->ip_addr, command->pid)->cmd);
    if (access(response, F_OK) != 0) {
      write(*cli_sockfd, "NaN", MAX_LEN);
    } else {
      FILE *f = fopen(response, "r");
      fseek(f, 0, SEEK_END);
      long length = ftell(f);
      sprintf(response, "%ld", length);
      fclose(f);
      write(*cli_sockfd, response, MAX_LEN);
    }
    free(response);
  }
}

void get_cont_handler(int *is_command, int *cli_sockfd,
                      Connection **connections, Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "sget", 4) == 0) {
    printf("[sget]:\n");
    *is_command = 1;
    // use resp temporaily to store the directory specified
    char *response = calloc(MAX_LEN, sizeof(char));
    char *fname = calloc(MAX_LEN, sizeof(char));
    // read range: [index * MAX_LEN, min(index * (MAXLEN + 1), FSIZE))
    long index;
    sscanf(command->cmd, "sget %ld %s", &index, fname);
    chdir(get_connection(connections, command->ip_addr, command->pid)->cmd);
    FILE *f = fopen(fname, "rb");
    printf("Reading from %ld in %s...\n", index * MAX_LEN, fname);
    fseek(f, index * MAX_LEN, SEEK_SET);
    fread(response, MAX_LEN, 1, f);
    printf("Read %ld...\n", strlen(response));
    write(*cli_sockfd, response, MAX_LEN);
    fclose(f);
    free(response);
    free(fname);
  }
}

void put_handler(int *is_command, int *cli_sockfd, Connection **connections,
                 Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "put", 3) == 0) {
    *is_command = 1;
    char *response = calloc(MAX_LEN, sizeof(char));
    char *fname = calloc(MAX_LEN, sizeof(char));
    long content_length = 0;
    sscanf(command->cmd, "put %ld %s", &content_length, fname);
    printf("Got notified for handling file %s(cl %ld)\n", fname,
           content_length);
    if (access(fname, F_OK) == 0) { // only check for existence
      sprintf(response, "File %s exists. Cleared.\n", fname);
    } else {
      sprintf(response, "File %s does not exist. Created.\n", fname);
    }
    printf("%s\n", file_name(fname));
    FILE *tmp = fopen(file_name(fname), "wb");
    fclose(tmp); // clear that file
    write(*cli_sockfd, response, MAX_LEN);
    free(response);
    free(fname);
  }
}

void put_cont_handler(int *is_command, int *cli_sockfd,
                      Connection **connections, Connection *command) {
  if (*is_command)
    return;
  if (strncmp(command->cmd, "sput", 4) == 0) {
    *is_command = 1;
    char *response = calloc(MAX_LEN, sizeof(char));
    char *fname = calloc(MAX_LEN, sizeof(char));
    long index = 0;
    long content_length = 0;
    long put_num = 0;
    sscanf(command->cmd, "sput %ld %ld %ld %s", &index, &put_num,
           &content_length, fname);
    FILE *f = fopen(file_name(fname), "ab");
    if (index == put_num - 1)
      fwrite(command->buf, content_length % MAX_LEN, 1, f);
    else
      fwrite(command->buf, MAX_LEN, 1, f);
    printf("Written %ld into %s...\n", strlen(command->buf), fname);
    fclose(f);
    sprintf(response, "%ld", strlen(command->buf));
    write(*cli_sockfd, response, MAX_LEN);
    free(response);
    free(fname);
  }
}

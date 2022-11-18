#include "client_utils.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  struct sockaddr_in addr;
  char command[MAX_LEN];
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  int isip = 0;
  ip = calloc(15, sizeof(char));
  do {
    printf("Input the server IP:");
    scanf("%s", ip);
    isip = is_ip(ip);
    if (!isip)
      printf("%s", red("Ip format not correct.\n"));
  } while (!isip);
  do {
    printf("Input the server port:");
    scanf("%ld", &port);
    if (port > 65535)
      printf("%s", red("Port not correct.\n"));
  } while (port > 65535);
  getchar();
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  signal(SIGINT, exit_handler);
  printf("Establishing connection with server...\n");
  init_connection(&addr);
  while (1) {
    printf("(ftp) %s>", get_pwd(&addr));
    bzero(command, MAX_LEN);
    if (fgets(command, MAX_LEN, stdin) == NULL) {
      printf("Fgets Error!\n");
      return -1;
    }
    command[strlen(command) - 1] = '\0';
    int is_command = 0;
    pwd_handler(&is_command, &addr, command);
    ls_handler(&is_command, &addr, command);
    cd_handler(&is_command, &addr, command);
    mkdir_handler(&is_command, &addr, command);
    remove_handler(&is_command, &addr, command);
    get_handler(&is_command, &addr, command);
    put_handler(&is_command, &addr, command);
    quit_handler(&is_command, &addr, command);
    unknown_command_handler(command);
  }
  return 0;
}

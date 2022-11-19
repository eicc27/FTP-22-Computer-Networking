#include "server_utils.h"

int main(int arg, char *argv[]) {
  Connection conn0 = {
      .ip_addr = -1,
      .pid = -1,
      .cmd = "init",
      .next_conn = NULL,
  };
  int ser_sockfd, cli_sockfd;
  struct sockaddr_in ser_addr, cli_addr;
  unsigned int ser_len, cli_len;
  // Initializes the current working directory(constant)
  char *cwd = calloc(MAX_LEN, sizeof(char));
  getcwd(cwd, MAX_LEN);
  Connection command;
  Connection *connections = init_connections(&conn0);
  bzero(&command, sizeof(command));
  if ((ser_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Sokcet Error!\n");
    return -1;
  }
  bzero(&ser_addr, sizeof(ser_addr));
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ser_addr.sin_port = htons(8989);
  ser_len = sizeof(ser_addr);
  if ((bind(ser_sockfd, (struct sockaddr *)&ser_addr, ser_len)) < 0) {
    printf("Bind Error!\n");
    return -1;
  }
  if (listen(ser_sockfd, 5) < 0) {
    printf("Linsten Error!\n");
    return -1;
  }
  bzero(&cli_addr, sizeof(cli_addr));
  cli_len = sizeof(cli_addr);
  print_init_info();
  while (1) {
    memset(&command, 0, sizeof(Connection));
    printf("server> ");
    if ((cli_sockfd =
             accept(ser_sockfd, (struct sockaddr *)&cli_addr, &cli_len)) < 0) {
      printf("Accept Error!\n");
      exit(1);
    }
    if (read(cli_sockfd, &command, sizeof(Connection)) < 0) {
      printf("Read Error!\n");
      exit(1);
    }
    char *cmd = command.cmd;
    printf("Recieved command from client: %s\n", cmd);
    printf("IPv4 Address: ");
    print_ip_in_str(command.ip_addr);
    printf("\nPID: %d\n", command.pid);
    int is_command = 0;
    init_handler(&is_command, &cli_sockfd, &connections, &command, cwd);
    quit_handler(&is_command, &cli_sockfd, &connections, &command);
    pwd_handler(&is_command, &cli_sockfd, &connections, &command);
    ls_handler(&is_command, &cli_sockfd, &connections, &command);
    cd_handler(&is_command, &cli_sockfd, &connections, &command);
    mkdir_handler(&is_command, &cli_sockfd, &connections, &command);
    remove_handler(&is_command, &cli_sockfd, &connections, &command);
    get_handler(&is_command, &cli_sockfd, &connections, &command);
    get_cont_handler(&is_command, &cli_sockfd, &connections, &command);
    put_handler(&is_command, &cli_sockfd, &connections, &command);
    put_cont_handler(&is_command, &cli_sockfd, &connections, &command);
  }
  return 0;
}
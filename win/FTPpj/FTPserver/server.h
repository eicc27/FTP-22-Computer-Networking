# include <stdio.h>
# include "command.h"
# include "util.h"

SOCKET initialize();
bool listenToClient(SOCKET);
void connectToClient(SOCKET);

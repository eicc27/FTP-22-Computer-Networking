#include "server.h"
int main() {
	printf("Hello!\n");
	SOCKET server_socket=initialize();
	if (server_socket != NULL) {
		printf("初始化完成\n");
		while (1) {
			connectToClient(server_socket);
		};
	}
	return 0;
}


SOCKET initialize() {
    printf("initializing ftp...\n");
	#ifdef _WIN32
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		printf("WSAStartup faild:%d\n", WSAGetLastError());
		return NULL;
	}
	
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serfd) {
		printf("socket faild:%d\n", WSAGetLastError());
		return NULL;
	}
    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));     //将＆addr中的前sizeof（addr）字节置为0，包括'\0'
    addr.sin_family = AF_INET;      //AF_INET代表TCP／IP协议
    addr.sin_addr.s_addr = ADDR_ANY; //将点间隔地址转换为网络字节顺序
    addr.sin_port = htons(8989);    //转换为网络字节顺序
	if (0 != bind(serfd, (struct sockaddr*)&addr, sizeof(addr))) {
		printf("bind faild:%d\n", WSAGetLastError());
		return NULL;
	};
	#endif


    return serfd;
}

bool listenToClient(SOCKET s) {
	char cmd[1024] = {0};
	int nRes = recv(s, cmd, 1024, 0);
	if (nRes == 0) {
		printf("客户端下线..%d\n", WSAGetLastError());
		return false;
	}
	else if(nRes < 0){
		printf("Recv faild.%d\n", WSAGetLastError());
		return false;
	}
	else {
		for_i_in_range(CMD_NUM) {
			if (!strcmp(cmd, commands[i].cmd)) {
				printf("客户端输入命令为:%s\n", commands[i].cmd);
				commands[i].function(s);
			}
		}
	}
	
	return true;
}

void connectToClient(SOCKET s) {
	printf("正在监听\n");
	//监听客户端链接
	if (0 != listen(s, 10)) {
		printf("listen faild:%d\n", WSAGetLastError());
		return NULL;
	}
	printf("等待客户端链接...\n");
	//有客户端链接，那么就要接受链接了
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	SOCKET clifd = accept(s, (struct sockaddr*)&cliAddr, &len);
	if (INVALID_SOCKET == clifd) {
		printf("accept faild:%d\n", WSAGetLastError());
		return NULL;
	}
	printf("客户端链接成功\n");
	while(listenToClient(clifd)) {
	}
	return;
}
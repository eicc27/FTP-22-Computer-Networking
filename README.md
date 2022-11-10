# FTP-22-Computer-Networking

A basic FTP impl done by Sihan Chen, Fangbo Zhao, Cheng Qian, and Shengjiu Dai.

Setup in linux

`make`

Set up in Windows

`gcc -c command.c util.c client.c -std=c99`

`gcc -o client.exe util.o command.o client.o`

`./client.exe`

## Revision \#1

### Client Revision
添加了各个命令的“筋骨”。*理论上*能够与服务器建立连接，读取服务器的返回，以及在客户端创建本地文件以及读写本地文件。另外添加了一些有用的注释，以及修改了一些头文件。

### Server TODOs
对于像是`ls` `pwd`这种只传递命令本身含义的指令，客户端会直接将原命令传给服务端；对于有参命令，服务端也需要一个这种指令解析的步骤。

### Client TODOs
`arg`中是不带指令名称本身的，在一些需要额外参数的传递中，需要附带指令名称本身。另外还有一些杂项处理，主要包含接收到服务器信息后该如何显示给用户。大部分都包含在各个函数的TODO中。

### Tests
未测试。
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "SocketPerson.h"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "服务端使用方法: \n";
        cout << "    ./demo-server 服务器端口\n";
        cout << "Example:\n";
        cout << "    ./demo-server 10001\n";
        return EXIT_FAILURE;
    }

    // 第一步: 创建服务器的socket
    int lisentFd = socket(AF_INET, SOCK_STREAM, 0);
    if (lisentFd == -1)
    {
        perror("Socket failed!");
        return EXIT_FAILURE;
    }

    // 第二步: 把服务器用于通信的IP和端口绑定到Socket上
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    // 指定协议
    serverAddr.sin_family = AF_INET;
    // 指定服务端的任意网卡的IP都可以用于通信
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 指定通信端口, 普通用户只能指定1024以上的端口
    serverAddr.sin_port = htons(atoi(argv[1]));
    // 绑定端口
    if (bind(lisentFd, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr)) != 0)
    {
        perror("bind failed");
        close(lisentFd);
        return EXIT_FAILURE;
    }

    // 第三步: 将socket设置为监听状态(可连接状态)
    if (listen(lisentFd, 5) != 0)
    {
        perror("listen failed");
        return EXIT_FAILURE;
    }

    // 第四步: 受理客户端的链接请求, 如果客户没有链接上来, 那么accept将会阻塞等待
    int clientFd = accept(lisentFd, 0, 0);
    if (clientFd == -1)
    {
        perror("accept failed");
        close(lisentFd);
        return -1;
    }

    cout << "客户端已连接\n";

    // 第五步: 与客户端通信, 接受客户端发来的报文后回复ok
    char buffer[1024];
    while (true)
    {
        int ret;
        memset(buffer, 0, sizeof(buffer));

        // 接受客户发来的报文, 如果没有发送请求报文则recv将会阻塞等待
        if ((ret = recv(clientFd, buffer, sizeof(buffer), 0)) <= 0)
        {
            fprintf(stderr, "%s %d: %s\n", "recv failed, ret = ", ret, strerror(errno));
            break;
        }

        cout << "接受: " << buffer << "\n";

        // 发送一个ok给客户端
        strcpy(buffer, "ok");
        if ((ret = send(clientFd, buffer, strlen(buffer), 0)) <= 0)
        {
            fprintf(stderr, "%s %d: %s", "send failed, ret = ", ret, strerror(errno));
            break;
        }
    }

    // 第六步: 关闭socket, 释放资源
    return EXIT_SUCCESS;
}
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "SocketPerson.h"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		cout << "客户端使用方法: \n";
		cout << "    ./demo-client 服务器IP 服务器端口\n";
		cout << "Example:\n";
		cout << "    ./demo-client 192.168.0.31 10001\n";
		return EXIT_FAILURE;
	}

	// 第1步：创建客户端的socket,
	//  AF_INET 指定指定的协议为IPv4,
	//  SOCK_STREM 指定使用面向连接的传输层协议
	//  0 表示根据前两个参数自动确定适用的, 所有值和协议的map在/etc/protocols中
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);

	// 第2步: 向服务器发起连接请求

	// 服务器IP的结构体, 系统API直接转换, 可以是IP地址也可以是域名, 系统会自动查询DNS进行转换
	hostent *host;
	if ((host = gethostbyname(argv[1])) == 0) {
		perror("gethostbyname failed!");
		return EXIT_FAILURE;
	}

	// 存放服务器IP和端口的结构体
	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(sockaddr_in));
	// 使用IPv4协议
	serverAddr.sin_family = AF_INET;
	memcpy(&serverAddr.sin_addr, host->h_addr, host->h_length);
	// 主机字节序转为网络字节序然后保存到结构体中, 网络字节序要求为大端序, 主机可能是小端序, 因此需要保存
	serverAddr.sin_port = htons(atoi(argv[2]));

	// 向服务器发起连接
	if ((connect(socketFd, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr))) != 0) {
		perror("Connection failed!");
		close(socketFd);
		return EXIT_FAILURE;
	}

	// 第3步：与服务端通讯，客户发送一个请求报文后等待服务端的回复，收到回复后，再发下一个请求报文
	char buffer[1024];
	Person people[3] = {
	    {1, "Jack Wang"},
	    {2, "Hong Shen"},
	    {3, "Shihong Wang"}};

	for (int i = 0; i < 3; i++) {
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "Person: ID=%d, Name=%s", people[i].id, people[i].name.c_str());

		// 向服务器发送报文
		int iret;
		if ((iret = send(socketFd, buffer, strlen(buffer), 0)) <= 0)
			fprintf(stderr, "%s %d %s: %s\n", "Send at", i, " times failed", strerror(errno));

		cout << "已发送: " << buffer << "\n";

		// 接受服务器发来的报文, 如果没有发来recv将会阻塞当前进程
		// 如果客户端断开连接, 那么就会返回0
		memset(buffer, 0, sizeof(buffer));
		if ((iret = recv(socketFd, buffer, sizeof(buffer), 0) <= 0)) {
			cout << "iret=" << iret << "\n";
			break;
		}

		cout << "已接受: " << buffer << "\n";

		sleep(1);
	}

	// 第四步: 关闭socket, 释放资源
	close(socketFd);

	return EXIT_SUCCESS;
}
#include <iostream>
#include <string>
#include <vector>

#include "SocketPerson.h"
#include "TcpServer.h"

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

using std::cout;
using std::string;
using std::to_string;

void parentExit(int signal);
void childExit(int signal);

TcpServer server;


int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "服务端使用方法: \n";
		cout << "    ./demo-server 服务器端口\n";
		cout << "Example:\n";
		cout << "    ./demo-server 10001\n";
		return EXIT_FAILURE;
	}

	// 忽略所有的信号, 避免打扰
	for (int i = 0; i <= 64; i++)
		signal(i, SIG_IGN);

	// 捕获SIGTERM和SIGINT信号
	signal(SIGINT, parentExit);
	signal(SIGTERM, parentExit);

	short serverListenPort = ::atoi(argv[1]);
	if (server.initialize(serverListenPort) == false) {
		perror("Server Intialize Failed!\n");
		return EXIT_FAILURE;
	}

	while (true) {
		// 每受理一个客户端连接
		if (server.accept() == false) {
			perror("accept()");
			return EXIT_FAILURE;
		}

		pid_t childPid = fork();
		if (childPid == -1) {
			perror("fork failed!");
			return EXIT_FAILURE;
		} else if (childPid > 0) {
			// 父进程
			server.closeConnect();
			cout << "监听进程Fork子进程: " << childPid << " 处理链接\n";
		} else {
			// 子进程
			signal(SIGTERM, childExit);
			signal(SIGINT, SIG_IGN);

			server.closeListen();

			cout << "客户端已链接, IP=" << server.getClientIP() << "\n";

			string buffer;
			while (true) {
				if (server.recv(buffer, 1024) == false) {
					perror("Server Receive Failed!\n");
					break;
				}
				cout << "接受: " << buffer << "\n";

				buffer = "ok";
				if (server.send(buffer) == false) {
					perror("Server Send Failed\n");
					break;
				}
				cout << "发送: " << buffer << "\n";
			}
			return EXIT_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}

// 父进程的信号处理函数
void parentExit(int sig) {
	// 防止信号处理过程中再次被中断
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	cout << "父进程退出, PID=" << getpid() << ", sig=" << sig << "\n";

	// 向所有的子进程发送SIGTERM指令, 通知他们退出
	kill(0, SIGTERM);

	// 释放全局资源的代码
	server.closeListen();

	// 退出
	exit(EXIT_SUCCESS);
}

void childExit(int sig) {
	// 避免再次被中断
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	cout << "子进程退出, PID=" << getpid() << ", sig=" << sig << "\n";

	// 释放子进程的资源
	server.closeConnect();

	// 退出
	exit(0);
}
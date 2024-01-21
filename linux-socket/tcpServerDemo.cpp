#include <iostream>
#include <string>
#include <vector>

#include "SocketPerson.h"
#include "TcpServer.h"

using std::cout;
using std::string;
using std::to_string;


int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "服务端使用方法: \n";
		cout << "    ./demo-server 服务器端口\n";
		cout << "Example:\n";
		cout << "    ./demo-server 10001\n";
		return EXIT_FAILURE;
	}

	short serverPort = ::atoi(argv[1]);
	TcpServer server(serverPort);
	if (server.initialize() == false) {
		perror("Server Intialize Failed!\n");
		return EXIT_FAILURE;
	}

	if (server.accept() == false) {
		perror("accept()");
		return EXIT_FAILURE;
	}
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
#include <iostream>
#include <string>
#include <vector>

#include "SocketPerson.h"
#include "TcpClient.h"

using std::cout;
using std::string;
using std::to_string;


int main(int argc, char *argv[]) {

	if (argc < 3) {
		cout << "客户端使用方法: \n";
		cout << "    ./demo-client 服务器IP 服务器端口\n";
		cout << "Example:\n";
		cout << "    ./demo-client 192.168.0.31 10001\n";
		return EXIT_FAILURE;
	}

	short serverPort = ::atoi(argv[2]);
	string serverAddr(argv[1]);
	TcpClient client(serverPort, serverAddr);

	if (!client.connect()) {
		perror("Client Connection Failed!\n");
		return EXIT_FAILURE;
	}

	string buffer;
	Person people[3] = {
	    {1, "Jack Wang"},
	    {2, "Hong Shen"},
	    {3, "Shihong Wang"}};

	for (size_t i = 0; i < 10; i++) {
		buffer = to_string(people[i % 3]) + " " + to_string(i);

		if (!client.send(buffer)) {
			fprintf(stderr, "%s %lu %s: %s\n", "Send at", i, " times failed", strerror(errno));
		}
		cout << "已发送: " << buffer << "\n";

		if (!client.receive(buffer, 1024)) {
			fprintf(stderr, "%s %lu %s: %s\n", "Receive at", i, " times failed", strerror(errno));
			break;
		}

		cout << "已接受: " << buffer << "\n";

		sleep(1);
	}

	return EXIT_SUCCESS;
}
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "FileInfo.h"
#include "TcpServer.h"


using std::cout;
using std::string;
using std::to_string;
namespace fs = std::filesystem;


int main(int argc, char *argv[]) {

	if (argc < 2) {
		cout << "服务端使用方法: \n";
		cout << "    ./fileReceiver 服务器端口\n";
		cout << "Example:\n";
		cout << "    ./fileReceiver 10001\n";
		return EXIT_FAILURE;
	}

	short port = ::atoi(argv[1]);
	TcpServer server(port);

	if (!server.initialize()) {
		perror("Server Intialize Failed!\n");
		return EXIT_FAILURE;
	}

	if (!server.accept()) {
		perror("accept()");
		return EXIT_FAILURE;
	}
	cout << "客户端已链接, IP=" << server.getClientIP() << "\n";


	// 发送文件的流程:
	//  1. 接受传送文件的大小和文件名
	//  2. 回复确认报文给客户端, 等待客户端数据到来
	//  3. 接受文件内容
	//  4. 接受完成后回复确认报文, 表示文件已经接收成功

	FileInfo fileInfo;
	if (!server.recv(&fileInfo, sizeof(fileInfo))) {
		perror("Receive FileInfo Failed!");
		return EXIT_FAILURE;
	}
	cout << "Receive FileInfo: \n"
	     << fileInfo;

	if (!server.send("ok")) {
		perror("Send FileInfo ACK Failed!");
		return EXIT_FAILURE;
	}

	fs::path filePath(fileInfo.fileName);
	fs::path outFilePath = fs::path(__FILE__).parent_path().parent_path().append("out");
	if (!fs::exists(outFilePath))
		fs::create_directory(outFilePath);
	outFilePath.append(filePath.filename().string());
	cout << "Start Receiving, writing to " << outFilePath << "\n";
	if (!server.recvFile(outFilePath.string(), fileInfo.fileSize)) {
		perror("Receive file failed!");
		return EXIT_FAILURE;
	}

	if (!server.send("ok")) {
		perror("Send File Receive ACK Failed!");
		return EXIT_FAILURE;
	}
	cout << "Successfully Received\n";

	return EXIT_SUCCESS;
}
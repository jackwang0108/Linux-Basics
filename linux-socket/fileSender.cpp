#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "FileInfo.h"
#include "TcpClient.h"


using std::cout;
using std::string;
using std::to_string;
namespace fs = std::filesystem;


int main(int argc, char *argv[]) {

	if (argc < 4) {
		cout << "文件传送客户端使用方法: \n";
		cout << "    ./fileSender 服务器IP 服务器端口 文件路径\n";
		cout << "Example:\n";
		cout << "    ./fileSender 192.168.0.31 10001 ./data.txt\n";
		return EXIT_FAILURE;
	}

	short port = ::atoi(argv[2]);
	string addr(argv[1]);
	TcpClient client(port, addr);

	if (!client.connect()) {
		perror("Client Connection Failed!\n");
		return EXIT_FAILURE;
	}

	// 发送文件的流程:
	//  1. 将传送文件的大小和文件名通知服务端
	//  2. 等待客户端确认文件大小和文件名
	//  3. 开始发送文件
	//  4. 等待服务端发送确认报文
	//  = TCP仅确保了数据包能到达, 但是无法保证对端一定处理了数据
	//  = 如果对方忙, 那么就还需要等待
	//  = 这里的确认是确保业务的可靠, 而非数据的可靠传输


	string fileName = argv[3];
	fs::path filePath = fs::absolute(fileName);
	uintmax_t fileSize = fs::file_size(filePath);
	FileInfo fileInfo{.fileSize = fileSize};
	strcpy(fileInfo.fileName, filePath.c_str());
	if (!client.send(&fileInfo, sizeof(fileInfo))) {
		perror("Send FileInfo Failed!");
		return EXIT_FAILURE;
	}
	cout << "Send FileInfo: \n"
	     << fileInfo;

	string serverInfo;
	if (!client.receive(serverInfo, 1024)) {
		perror("File Info ACK Failed!");
		return EXIT_FAILURE;
	}
	cout << "ok\n";

	cout << "Start Sending\n";
	if (!client.sendFile(fileInfo.fileName, fileInfo.fileSize)) {
		perror("Send File Content Failed!");
		return EXIT_FAILURE;
	}

	if (!client.receive(serverInfo, 1024)) {
		perror("File Content ACK Failed!");
		return EXIT_FAILURE;
	}
	cout << "Successfully Sended\n";


	return EXIT_SUCCESS;
}
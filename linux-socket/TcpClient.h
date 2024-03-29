#pragma once
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


class TcpClient {
private:
	int clientConnectFd = -1;
	short serverPort = 0;
	std::string serverAddr = "";


public:
	TcpClient(short serverPort, std::string &serverAddr)
	    : serverPort(serverPort), serverAddr(serverAddr) {
	}

	bool connect();

	bool send(const std::string &buffer);

	template<typename T>
	bool send(const T *data, size_t size);

	bool sendFile(const std::string &fileName, size_t size);

	bool receive(std::string &buffer, int maxLength);

	bool close();

	~TcpClient() {
		close();
	}
};

bool TcpClient::connect() {
	// 已经连接
	if (!clientConnectFd == -1)
		return false;

	// 创建socket
	if ((clientConnectFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		std::cerr << "Open socket failed!\n";
		return false;
	}

	// 解析服务器IP地址
	hostent *host;
	if ((host = gethostbyname(serverAddr.c_str())) == 0) {
		std::cerr << "Gethostbyname error!\n";
		::close(clientConnectFd);
		return false;
	}

	sockaddr_in serverSockAddr;
	std::memset(&serverSockAddr, 0, sizeof(sockaddr_in));
	// 设置服务器通信协议
	serverSockAddr.sin_family = AF_INET;
	// 设置服务器IP地址
	std::memcpy(&serverSockAddr.sin_addr, host->h_addr, host->h_length);
	// 设置服务器通信端口
	serverSockAddr.sin_port = htons(serverPort);

	// 发起连接
	if (::connect(clientConnectFd, reinterpret_cast<const sockaddr *>(&serverSockAddr), sizeof(serverSockAddr)) != 0) {
		std::cerr << "Connect to server failed!\n";
		::close(clientConnectFd);
		return false;
	}

	return true;
}

bool TcpClient::send(const std::string &buffer) {
	if (clientConnectFd == -1)
		return false;
	return ::send(clientConnectFd, buffer.data(), buffer.size(), 0) > 0;
}


template<typename T>
bool TcpClient::send(const T *data, size_t size) {
	if (clientConnectFd == -1)
		return false;
	return ::send(clientConnectFd, data, size, 0) > 0;
}


bool TcpClient::sendFile(const std::string &fileName, size_t size) {
	std::ifstream infile(fileName, std::ios::binary);
	if (!infile.is_open()) {
		std::cout << "打开文件 " << fileName << " 出错";
		return false;
	}

	char buffer[32];
	int onRead = 0, totalBytes = 0;
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		onRead = (size - totalBytes > 32) ? 32 : size - totalBytes;
		infile.read(buffer, onRead);

		if (!send(buffer, onRead))
			return false;

		totalBytes += onRead;
		if (totalBytes == size)
			break;
	}
	return true;
}

bool TcpClient::receive(std::string &buffer, int maxLength) {
	buffer.clear();
	buffer.resize(maxLength);
	int readSize = ::recv(clientConnectFd, &buffer[0], buffer.size(), 0);
	if (readSize <= 0) {
		buffer.clear();
		return false;
	}
	buffer.resize(readSize);
	return true;
}

bool TcpClient::close() {
	if (clientConnectFd == -1)
		return false;
	::close(clientConnectFd);
	clientConnectFd = -1;
	return true;
}

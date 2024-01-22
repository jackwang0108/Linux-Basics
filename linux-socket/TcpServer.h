#pragma once

#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


class TcpServer {
private:
	int serverListenFd = -1;
	int serverConnectFd = -1;
	unsigned short listenPort = 0;
	std::string clientIP = "";

public:
	TcpServer() {}
	TcpServer(short serverPort) : listenPort(serverPort) {
	}

	bool initialize(unsigned short listenPort, int maxMessageQueueSize) noexcept;

	bool accept();

	bool send(const std::string &buffer);

	bool recv(std::string &buffer, const size_t maxLength);

	bool closeListen();

	bool closeConnect();

	const std::string &getClientIP() { return clientIP; }

	~TcpServer() {
		closeConnect();
		closeListen();
	}
};


bool TcpServer::initialize(unsigned short listenPort = 0, int maxMessageQueueSize = 5) noexcept {
	if (listenPort != 0)
		this->listenPort = listenPort;
	// 创建socket
	serverListenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverListenFd == -1) {
		perror("Socket open failed!");
		return false;
	}

	// 绑定IP和端口到socket
	sockaddr_in serverSocketAddr;
	std::memset(&serverSocketAddr, 0, sizeof(serverSocketAddr));
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverSocketAddr.sin_port = htons(this->listenPort);
	if (bind(serverListenFd, reinterpret_cast<const sockaddr *>(&serverSocketAddr), sizeof(serverSocketAddr)) != 0) {
		perror("Server bind IP and Port failed!");
		::close(serverListenFd);
		serverListenFd = -1;
		return false;
	}

	// 设置socket为监听状态
	if (listen(serverListenFd, maxMessageQueueSize) != 0) {
		perror("Set socket to listen failed!");
		::close(serverListenFd);
		serverListenFd = -1;
		return false;
	}

	return true;
}

bool TcpServer::accept() {
	sockaddr_in clientSocketAddr;
	socklen_t clientSocketAddrLen = sizeof(clientSocketAddr);
	if ((serverConnectFd = ::accept(serverListenFd, reinterpret_cast<sockaddr *>(&clientSocketAddr), &clientSocketAddrLen)) == -1)
		return false;

	clientIP = inet_ntoa(clientSocketAddr.sin_addr);
	return true;
}

bool TcpServer::send(const std::string &buffer) {
	if (serverConnectFd == -1)
		return false;

	return ::send(serverConnectFd, buffer.data(), buffer.size(), 0) > 0;
}

bool TcpServer::recv(std::string &buffer, const size_t maxLength) {
	buffer.size();
	buffer.resize(maxLength);
	int readSize = ::recv(serverConnectFd, &buffer[0], buffer.size(), 0);
	if (readSize <= 0) {
		buffer.clear();
		return false;
	}
	buffer.resize(readSize);
	return true;
}

bool TcpServer::closeListen() {
	if (serverListenFd == -1)
		return false;
	::close(serverListenFd);
	serverListenFd = -1;
	return true;
}


bool TcpServer::closeConnect() {
	if (serverConnectFd == -1)
		return false;
	::close(serverConnectFd);
	serverConnectFd = -1;
	return true;
}
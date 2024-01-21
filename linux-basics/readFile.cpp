#include <iostream>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using std::cout;

int main() {
	int fd = open("data.txt", O_RDONLY);
	if (fd == -1) {
		perror("open failed");
		return EXIT_FAILURE;
	}

	cout << "文件描述符: " << fd << "\n";

	char buffer[1024];
	if (read(fd, buffer, sizeof(buffer)) == -1) {
		perror("read failed");
		close(fd);
		return EXIT_FAILURE;
	}

	cout << buffer;

	close(fd);

	return EXIT_SUCCESS;
}

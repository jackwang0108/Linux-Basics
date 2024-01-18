#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using std::cout;

int main()
{
    int fd = open("data.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }

    cout << "文件描述符: " << fd << "\n";

    char buffer[1024];
    sprintf(buffer, "%s %d %s\n", "你好, 这是来自进程编号", getpid(), "写入的数据");

    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        perror("write failed");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    return EXIT_SUCCESS;
}
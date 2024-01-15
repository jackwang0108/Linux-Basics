#include <iostream>

#include <stdlib.h>
#include <unistd.h>

using std::cout;
using std::endl;

// Linux提供了system()函数和exec函数族，在C++程序中，可以执行其它的程序（二进制文件、操作系统命令或Shell脚本）。

// 一、system()函数
// system()函数提供了一种简单的执行程序的方法，把需要执行的程序和参数用一个字符串传给system()函数就行了。
// 函数的声明：
// int system(const char * string);
// system()函数的返回值比较麻烦。
// 1）如果执行的程序不存在，system()函数返回非0；
// 2）如果执行程序成功，并且被执行的程序终止状态是0，system()函数返回0；
// 3）如果执行程序成功，并且被执行的程序终止状态不是0，system()函数返回非0。
void testSystem()
{
    int ret = system("ls -alh");
    cout << "ret=" << ret << endl;
}

// 二、exec函数族
// exec函数族提供了另一种在进程中调用程序（二进制文件或Shell脚本）的方法。
// exec函数族的声明如下：
// = int execl(const char *path, const char *arg, ...);
// int execlp(const char *file, const char *arg, ...);
// int execle(const char *path, const char *arg,...,char * const envp[]);
// = int execv(const char *path, char *const argv[]);
// int execvp(const char *file, char *const argv[]);
// int execvpe(const char *file, char *const argv[],char *const envp[]);
// 注意：
// 1）如果执行程序失败则直接返回-1，失败原因存于errno中。
// 2）新进程的进程编号与原进程相同，但是，新进程取代了原进程的代码段、数据段和堆栈。
// 3）如果执行成功则函数不会返回，当在主程序中成功调用exec后，被调用的程序将取代调用者程序，也就是说，exec函数之后的代码都不会被执行。
// 4）在实际开发中，最常用的是execl()和execv()，其它的极少使用
void testExec()
{
    char *args[] = {(char *)"/bin/ls", (char *)"-lt", (char *)"/home/parallels", 0};

    // int ret = execv("/bin/ls", args);
    cout << "PID=" << getpid() << endl;
    int ret = execv("./bin/getpid", args);
}

int main(int argc, char *argv[])
{
    testSystem();
    testExec();
    cout << "Returned in main" << endl;
    return 0;
}
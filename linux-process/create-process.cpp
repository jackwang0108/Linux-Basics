#include <fstream>
#include <iostream>

#include <unistd.h>

using std::cout;
using std::endl;

// 一、Linux的0、1和2号进程
// 整个linux系统全部的进程是一个树形结构。
// 0号进程（系统进程）是所有进程的祖先，它创建了1号和2号进程。
// 1号进程（systemd）负责执行内核的初始化工作和进行系统配置。
// 2号进程（kthreadd）负责所有内核线程的调度和管理。
// 用pstree命令可以查看进程树（yum -y install psmisc）。
// pstree -p 进程编号

// 二、进程标识
// 每个进程都有一个非负整数表示的唯一的进程ID。虽然是唯一的，但是进程ID可以复用。当一个进程终止后，其进程ID就成了复用的候选者。Linux采用延迟复用算法，让新建进程的ID不同于最近终止的进程所使用的ID。这样防止了新进程被误认为是使用了同一ID的某个已终止的进程。
// pid_t getpid(void);     // 获取当前进程的ID。
// pid_t getppid(void);    // 获取父进程的ID。
void testGetpid() {
	cout << "Current PID: " << getpid() << endl;
	cout << "Parent PID: " << getppid() << endl;
}

// 三、fork()函数
// 一个现有的进程可以调用fork()函数创建一个新的进程。
// pid_t fork(void);
// 由fork()创建的新进程被称为子进程。子进是父进程的副本，父进程和子进程都从调用fork()之后的代码开始执行。
// fork()函数被调用一次，但返回两次。两次返回的区别是子进程的返回值是0，而父进程的返回值则是子进程的进程ID。
// 子进程获得了父进程数据空间、堆和栈的副本（注意：子进程拥有的是副本，不是和父进程共享）。

// 四、fork()的两种用法
// 1）父进程复制自己，然后，父进程和子进程分别执行不同的代码。这种用法在网络服务程序中很常见，父进程等待客户端的连接请求，当请求到达时，父进程调用fork()，让子进程处理些请求，而父进程则继续等待下一个连接请求。
// 2）进程要执行另一个程序。这种用法在Shell中很常见，子进程从fork()返回后立即调用exec。
void testFork() {
	cout << "Before Fork" << endl;
	pid_t pid = fork();
	if (pid == 0) {
		cout << "子进程, PID=" << getpid() << endl;
	} else {
		cout << "父进程, PID=" << getpid() << ", 子进程 PID=" << pid << endl;
		exit(0);
	}
}

// fork()的一个特性是在父进程中打开的文件描述符都会被复制到子进程中，父进程和子进程共享同一个文件偏移量。
// 如果父进程和子进程写同一描述符指向的文件，但又没有任何形式的同步，那么它们的输出可能会相互混合。
void testForkWrite() {
	std::ofstream outputFile("file.txt");
	pid_t childPID = fork();
	if (childPID == 0) {
		sleep(1);
		for (int i = 0; i < 100; i++)
			outputFile << "Written by child, PID: " << getpid() << ", idx: " << i << endl;
		exit(0);
	} else {
		sleep(1);
		for (int i = 0; i < 100; i++)
			outputFile << "Written by parent, PID: " << getpid() << ", idx: " << i << endl;
	}
}

// 六、vfork()函数
// vfork()函数的调用和返回值与fork()相同，但两者的语义不同。
// vfork()函数用于创建一个新进程，而该新进程的目的是exec一个新程序，它不复制父进程的地址空间，因为子进程会立即调用exec，于是也就不会使用父进程的地址空间。如果子进程使用了父进程的地址空间，可能会带来未知的结果。
// vfork()和fork()的另一个区别是：vfork()保证子进程先运行，在子进程调用exec或exit()之后父进程才恢复运行。
// The intent of vfork was to eliminate the overhead of copying the whole process image if you only want to do an exec* in the child. Because exec* replaces the whole image of the child process, there is no point in copying the image of the parent.
// if ((pid = vfork()) == 0) {
//   execl(..., NULL); /* after a successful execl the parent should be resumed */
//   _exit(127); /* terminate the child in case execl fails */
// }
// With most current kernels, however, including Linux, the primary benefit of vfork has disappeared because of the way fork is implemented. Rather than copying the whole image when fork is executed, copy-on-write techniques are used.

int main(int argc, char *argv[]) {
	testGetpid();
	testFork();
	testForkWrite();
	return 0;
}
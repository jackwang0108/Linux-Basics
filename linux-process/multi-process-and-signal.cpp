#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

using std::cout;
using std::endl;

// Linux操作系统提供了kill和killall命令向进程发送信号，在程序中，可以用kill()函数向其它进程发送信号。
// 函数声明：
//      int kill(pid_t pid, int sig);
// kill()函数将参数sig指定的信号给参数pid 指定的进程。
// 参数pid 有几种情况：
//      1）pid>0 将信号传给进程号为pid 的进程。
//      2）pid=0 将信号传给和当前进程相同进程组的所有进程，常用于父进程给子进程发送信号，注意，发送信号者进程也会收到自己发出的信号。
//      3）pid=-1 将信号广播传送给系统内所有的进程，例如系统关机时，会向所有的登录窗口广播关机信息。
// sig：准备发送的信号代码，假如其值为0则没有任何信号送出，但是系统会执行错误检查，通常会利用sig值为零来检验某个进程是否仍在运行。
// 返回值说明： 成功执行时，返回0；失败返回-1，errno被设置。

// 在多进程的服务程序中，如果子进程收到退出信号，子进程自行退出，如果父进程收到退出信号，则应该先向全部的子进程发送退出信号，然后自己再退出。

// 父进程的信号处理函数
void parentEXIT(int signal);
// 子进程的信号处理函数
void childEXIT(int signal);

int main(int argc, char *argv[]) {
	// 忽略所有信号, 避免被打扰
	for (int i = 0; i <= 64; i++)
		signal(i, SIG_IGN);

	// 捕捉SIGTERM信号和SIGINT信号
	signal(SIGINT, parentEXIT);
	signal(SIGTERM, parentEXIT);

	while (true) {
		if (fork() > 0) {
			cout << "父进程运行中" << endl;
			sleep(5);
		} else {
			// 子进程重新设置信号
			signal(SIGTERM, childEXIT);
			signal(SIGINT, SIG_IGN);
			while (true) {
				cout << "子进程运行中, PID=" << getpid() << endl;
				sleep(1);
			}
		}
	}

	return 0;
}

// 父进程的信号处理函数
void parentEXIT(int sig) {
	// 防止信号处理过程中再次被中断
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	cout << "父进程退出, PID=" << getpid() << ", sig=" << sig << endl;

	// 向所有的子进程发送SIGTERM指令, 通知他们退出
	kill(0, SIGTERM);

	// 释放全局资源的代码

	// 退出
	exit(0);
}

void childEXIT(int sig) {
	// 避免再次被中断
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	cout << "子进程退出, PID=" << getpid() << ", sig=" << sig << endl;

	// 释放子进程的资源

	// 退出
	exit(0);
}
#include <iostream>

#include <unistd.h>

// 一、信号的基本概念 
// 信号（signal）是软件中断，是进程之间相互传递消息的一种方法，用于通知进程发生了事件，但是，不能给进程传递任何数据。
// 信号产生的原因有很多，在Shell中，可以用kill和killall命令发送信号：
// kill -信号的类型 进程编号
// killall -信号的类型 进程名

// 二、信号的类型 
// 信号名	信号值	默认处理动作	发出信号的原因
// SIGHUP	1	A	终端挂起或者控制进程终止
// SIGINT	2	A	键盘中断Ctrl+c
// SIGQUIT	3	C	键盘的退出键被按下
// SIGILL	4	C	非法指令
// SIGABRT	6	C	由abort(3)发出的退出指令
// SIGFPE	8	C	浮点异常
// SIGKILL	9	AEF	采用kill -9 进程编号 强制杀死程序。
// SIGSEGV	11	CEF	无效的内存引用（数组越界、操作空指针和野指针等）。
// SIGPIPE	13	A	管道破裂，写一个没有读端口的管道。
// SIGALRM	14	A	由闹钟alarm()函数发出的信号。
// SIGTERM	15	A	采用“kill 进程编号”或“killall 程序名”通知程序。
// SIGUSR1	10	A	用户自定义信号1
// SIGUSR2	12	A	用户自定义信号2
// SIGCHLD	17	B	子进程结束信号
// SIGCONT	18		进程继续（曾被停止的进程）
// SIGSTOP	19	DEF	终止进程
// SIGTSTP	20	D	控制终端（tty）上按下停止键
// SIGTTIN	21	D	后台进程企图从控制终端读
// SIGTTOU	22	D	后台进程企图从控制终端写
// 其它	<=64	A	自定义信号
// 处理动作一项中的字母含义如下：
// A 缺省的动作是终止进程。
// B 缺省的动作是忽略此信号，将该信号丢弃，不做处理。
// C 缺省的动作是终止进程并进行内核映像转储（core dump）。
// D 缺省的动作是停止进程，进入停止状态的程序还能重新继续，一般是在调试的过程中。
// E 信号不能被捕获。
// F 信号不能被忽略。

int main()
{
    while (true)
    {
        std::cout << "任务执行了一次" << std::endl;
        sleep(1);
    }
    return 0;
}
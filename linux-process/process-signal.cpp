#include <iostream>

#include <unistd.h>
#include <signal.h>

using std::cout;
using std::endl;

// 进程对信号的处理方法有三种：
// 1）对该信号的处理采用系统的默认操作，大部分的信号的默认操作是终止进程。
// 2）设置信号的处理函数，收到信号后，由该函数来处理。
// 3）忽略某个信号，对该信号不做任何处理，就像未发生过一样。
// signal()函数可以设置程序对信号的处理方式。
// 函数声明：
// sighandler_t signal(int signum, sighandler_t handler);
// 参数signum表示信号的编号（信号的值）。
// 参数handler表示信号的处理方式，有三种情况：
// 1）SIG_DFL：恢复参数signum信号的处理方法为默认行为。 
// 2）一个自定义的处理信号的函数，函数的形参是信号的编号。
// 3）SIG_IGN：忽略参数signum所指的信号。

// 四、信号有什么用
// 服务程序运行在后台，如果想让中止它，杀掉不是个好办法，因为进程被杀的时候，是突然死亡，没有安排善后工作。
// 如果向服务程序发送一个信号，服务程序收到信号后，调用一个函数，在函数中编写善后的代码，程序就可以有计划的退出。
// 常见的例子就是释放网络连接, 数据库链接, 保存配置文件...
// 如果向服务程序发送0的信号，可以检测程序是否存活。

void func(int signal)
{
    cout << "收到了信号: " << signal << endl;
}

void alarm_func(int signal)
{
    cout << "Alarmed!" << endl;
}

int main(int argc, char *argv[])
{
    signal(1, func);
    // 使用宏增强可读性
    signal(SIGTERM, SIG_IGN);
    signal(SIGALRM, alarm_func);

    // 5 秒后程序向自己发送alarm信号
    alarm(5);
    while (true)
    {
        cout << "执行了一次任务" << endl;
        sleep(1);
    }
    return 0;
}
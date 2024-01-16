#include <iostream>

#include <unistd.h>

using std::cout;
using std::endl;
using std::string;

// 有8种方式可以中止进程，其中5种为正常终止，它们是：
// 1）在main()函数用return返回；
// 2）在任意函数中调用exit()函数；
// 3）在任意函数中调用_exit()或_Exit()函数；
// 4）最后一个线程从其启动例程（线程主函数）用return返回；
// 5）在最后一个线程中调用pthread_exit()返回；
// 异常终止有3种方式，它们是：
// 6）调用abort()函数中止；
// 7）接收到一个信号；
// 8）最后一个线程对取消请求做出响应。

// 一、进程终止的状态
// 在main()函数中，return的返回值即终止状态，如果没有return语句或调用exit()，那么该进程的终止状态是0。
// 在Shell中，查看进程终止的状态：echo $?
// 正常终止进程的3个函数（exit()和_Exit()是由ISO C说明的，_exit()是由POSIX说明的）。
// void exit(int status);
// void _exit(int status);
// void _Exit(int status);
// status也是进程终止的状态。
// 如果进程被异常终止，终止状态为非0。  服务程序的调度、日志和监控

// 二、资源释放的问题
// retun表示函数返回，会调用局部对象的析构函数，main()函数中的return还会调用全局对象的析构函数。
// exit()表示终止进程，不会调用局部对象的析构函数，只调用全局对象的析构函数。
// exit()会执行清理工作，然后退出，_exit()和_Exit()直接退出，不会执行任何清理工作。

// 三、进程的终止函数
// 进程可以用atexit()函数登记终止函数（最多32个），这些函数将由exit()自动调用。
// int atexit(void (*function)(void));
// exit()调用终止函数的顺序与登记时相反。 进程退出前的收尾工作

class A
{
private:
    string name;

public:
    A(const string &name) : name(name){};
    ~A() { cout << "调用 " << name << "的析构函数" << endl; };
};

void print()
{
    cout << "In Exit" << endl;
}

A a1("A1");

int main(int argc, char *argv[])
{
    A a2("A2");
    atexit(print);
    // return 0;
    // exit(0);
    _exit(0);
}
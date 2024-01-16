#include <iostream>

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using std::cout;
using std::endl;

// 如果父进程比子进程先退出，子进程将被1号进程托管（这也是一种让程序在后台运行的方法）。
// 如果子进程比父进程先退出，而父进程没有处理子进程退出的信息，那么，子进程将成为僵尸进程。
void testOprand()
{
    pid_t childPID = fork();
    if (childPID == 0)
    {
        for (int i = 0; i < 100; i++)
        {
            cout << "子进程 " << getpid() << " 正在运行中" << endl;
            sleep(1);
        }
    }
    else
    {
        sleep(5);
        cout << "父进程: " << getpid() << "运行结束" << endl;
    }
}

// 僵尸进程有什么危害？内核为每个子进程保留了一个数据结构，包括进程编号、终止状态、使用CPU时间等。父进程如果处理了子进程退出的信息，内核就会释放这个数据结构，父进程如果没有处理子进程退出的信息，内核就不会释放这个数据结构，子进程的进程编号将一直被占用。系统可用的进程编号是有限的，如果产生了大量的僵尸进程，将因为没有可用的进程编号而导致系统不能产生新的进程。
void testZombie()
{
    pid_t childPID = fork();
    if (childPID == 0)
    {
        sleep(1);
        cout << "子进程运行结束";
    }
    else
    {
        for (int i = 0; i < 100; i++)
        {
            cout << "父进程正在运行中" << endl;
            sleep(1);
        }
    }
}

// 僵尸进程的避免：
// 1）子进程退出的时候，内核会向父进程发头SIGCHLD信号，如果父进程用signal(SIGCHLD,SIG_IGN)通知内核，表示自己对子进程的退出不感兴趣，那么子进程退出后会立即释放数据结构。
// 2）父进程通过wait()/waitpid()等函数等待子进程结束，在子进程退出之前，父进程将被阻塞待。
// pid_t wait(int *stat_loc);
// pid_t waitpid(pid_t pid, int *stat_loc, int options);
// pid_t wait3(int *status, int options, struct rusage *rusage);
// pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
// 返回值是子进程的编号。
// stat_loc是子进程终止的信息：a）如果是正常终止，宏WIFEXITED(stat_loc)返回真，宏WEXITSTATUS(stat_loc)可获取终止状态；b）如果是异常终止，宏WTERMSIG(stat_loc)可获取终止进程的信号。
// 3）如果父进程很忙，可以捕获SIGCHLD信号，在信号处理函数中调用wait()/waitpid()。

void preventZombie1()
{
    signal(SIGCHLD, SIG_IGN);
    pid_t childPID = fork();
    if (childPID == 0)
    {
        sleep(1);
        cout << "子进程运行结束";
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            cout << "父进程正在运行中" << endl;
            sleep(1);
        }
    }
}

void preventZombie2()
{
    pid_t childPID = fork();
    if (childPID == 0)
    {
        sleep(100);
        cout << "子进程运行结束";
        // exit(1);
        abort();
    }
    else
    {
        int sts;
        pid_t childExitedPID = wait(&sts);
        cout << "子进程已终止, PID=" << childExitedPID << endl;
        // 返回值和异常退出是两个概念, 五个退出方式都被视为正常退出, 剩下三个都视为异常退出, WEXITSTATUS获得返回值
        if (WIFEXITED(sts))
            cout << "子进程正常退出, sts=" << sts << ", 退出状态为: " << WEXITSTATUS(sts) << ", 终止信号为: " << WTERMSIG(sts) << endl;
        else
            cout << "子进程异常退出, sts=" << sts << ", 退出状态为: " << WEXITSTATUS(sts) << ", 终止信号为: " << WTERMSIG(sts) << endl;
    }
}

// 父进程通常不会等待子进程运行结束, 因此往往捕获子进程终止信号, 而后再进行收尾工作
void processChild(int signal)
{
    int sts;
    pid_t childPID = wait(&sts);
    cout << "父进程捕获信号: " << signal << endl;
    cout << "子进程PID=" << childPID << ", sts=" << sts << ", 退出状态为: " << WEXITSTATUS(sts) << ", 终止信号为: " << WTERMSIG(sts) << endl;
}

int preventZombie3()
{
    signal(SIGCHLD, processChild);
    pid_t childPID = fork();
    if (childPID == 0)
    {
        cout << "子进程正在运行" << endl;
        sleep(5);
        int *p = 0;
        *p = 10;
        exit(0);
    }
    else
    {
        while (true)
        {
            cout << "父进程正在运行" << endl;
            sleep(1);
        }
    }
}

int main(int argc, char *argv[])
{
    // testOprand();
    // testZombie();
    // preventZombie1();
    // preventZombie2();
    preventZombie3();
    return 0;
}
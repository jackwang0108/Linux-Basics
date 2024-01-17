#include <cstring>
#include <iostream>

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "Person.h"
#include "Semephore.h"
#include "CircularQueue.h"

using std::cout;
using std::endl;

// C++ 标准库中的thread, mutex, condition_variable只支持了线程级别的多道程序
// 如果想要实现进程级别的多道程序处理, 需要使用操作系统提供的API

int main(int argc, char *argv[])
{
    using ElemType = Person;

    // 申请共享内存空间
    int shmID = shmget(0x5005, sizeof(CircularQueue<Person, 10>), 0640 | IPC_CREAT);
    if (shmID == -1)
    {
        perror("shmget failed!");
        return EXIT_FAILURE;
    }

    // 添加到当前进程的地址空间
    CircularQueue<Person, 10> *queue = (CircularQueue<Person, 10> *)shmat(shmID, 0, 0);
    if (queue == (void *)0)
    {
        perror("shmat failed!");
        return EXIT_FAILURE;
    }
    queue->init();

    // 使用共享内存前需要先声明锁
    // 互斥锁, 确保资源唯一访问
    Semaphore mutex;
    if (mutex.init(0x5001) == false)
    {
        perror("mutex init failed!");
        return EXIT_FAILURE;
    }
    // 信号量, 表示队列中的元素个数
    Semaphore cond;
    cond.init(0x5002, 0, 0);

    // 生产资源
    int idx = 0;
    while (idx <= 20)
    {
        // 临界区进入
        while (true)
        {
            mutex.wait();
            // 队列满则等待
            if (queue->full())
                mutex.post();
            else
                break;
        }

        Person p;
        p.no = idx;
        // 进入临界区
        sprintf(p.name, "JackWang %d", idx);
        if (queue->push(p))
        {
            cout << "元素入队 " << idx << endl;
            idx++;
        }
        // 退出临界区
        mutex.post();
        cond.post(1);
    }

    // 分离共享内存
    if (shmdt(queue) == -1)
    {
        perror("shmdt failed!");
        return EXIT_FAILURE;
    }

    // 删除共享内存
    if (shmctl(shmID, IPC_RMID, 0) == -1)
    {
        perror("shmctl failed!");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
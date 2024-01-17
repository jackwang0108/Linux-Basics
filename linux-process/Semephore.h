#pragma once

#include <sstream>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>

class Semaphore
{
private:
    // 用于信号量操作的共同体
    union semun
    {
        int val;
        semid_ds *buf;
        unsigned short *arry;
    };
    // 信号量id（描述符）
    int semID;
    // 如果把sem_flg设置为SEM_UNDO，操作系统将跟踪进程对信号量的修改情况，
    // 在全部修改过信号量的进程（正常或异常）终止后，操作系统将把信号量恢复为初始值。
    // 如果信号量用于互斥锁，设置为SEM_UNDO。
    // 如果信号量用于生产消费者模型，设置为0。
    short semFlag;

    Semaphore(const Semaphore &) = delete;
    Semaphore &operator=(const Semaphore &) = delete;

public:
    Semaphore() : semID(-1) {}
    ~Semaphore() {}
    bool init(key_t key, unsigned short value = 1, short semflag = SEM_UNDO)
    {
        // 如果信号量存在, 则获取信号量; 如果信号量不存在, 则创建并初始化值为value
        if (semID != -1)
            return false;
        // 初始化信号量
        semFlag = semflag;
        // 信号量的初始化不能直接用semget(key, 1, 0666|IPC_CREAT)
        // 因为信号量创建之后, 初始值为0, 如果用于互斥锁的话, 需要把值设置为1
        // 但是获取信号量则不需要创建初始值, 因此获取信号量和创建信号量的流程不同

        // 信号量的初始化分三个步骤：
        //  1）获取信号量，如果成功，函数返回。
        //  2）如果失败，则创建信号量。
        //  3) 设置信号量的初始值。

        // 获取信号量
        if ((semID = semget(key, 1, 0666)) == -1)
        {
            // 信号量不存在, 创建
            if (errno = ENOENT)
                // 这里 IPC_EXCL 将会确保创建成功, 如果信号量已经存在, 则会失败并设置errno
                if ((semID = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
                {
                    // 这里再次判断信号量是否已经存在, 因为第一次判断和创建之间不是原子操作, 可能会被中断
                    if (errno == EEXIST)
                    {
                        // 如果当前进程被其他进程中断, 并且其他进程已经成功创建了信号量, 则获取并返回, 否则失败
                        if ((semID = semget(key, 1, 0666)) == 1)
                        {
                            std::stringstream oss;
                            oss << "init semephore in PID=" << getpid() << " failed, interrupted by other process but still failed!";
                            perror(oss.str().c_str());
                            return false;
                        }
                        return true;
                    }
                    else
                    {
                        // 其他错误同样直接返回
                        std::stringstream oss;
                        oss << "init semephore in PID=" << getpid() << "failed, creating in the process fail!";
                        perror(oss.str().c_str());
                        return false;
                    }
                }

            // 信号量创建成功了, 初始化信号量的值
            semun sem_union;
            sem_union.val = value;
            // SETVAL和sem_union配合使用, 设置信号量的初始值
            if (semctl(semID, 0, SETVAL, sem_union) < 0)
            {
                perror("initialize semephore failed!");
                return false;
            }
        }
        return true;
    }

    bool wait(short value = -1)
    {
        if (semID == -1)
            return false;
        sembuf semBuf;
        // 信号量编号，0代表第一个信号量
        semBuf.sem_num = 0;
        // P操作的value必须小于0
        semBuf.sem_op = value;
        semBuf.sem_flg = semFlag;
        // int semop(int semid, struct sembuf *sops, size_t nsops);
        // semop() performs operations on selected semaphores in the set indicated by semid.  Each of the nsops elements in the array pointed
        // to by sops is a structure that specifies an operation to be performed on a single semaphore.  The elements of this  structure  are
        // of type struct sembuf, containing the following members:
        //    unsigned short sem_num;  /* semaphore number */
        //    short          sem_op;   /* semaphore operation */
        //    short          sem_flg;  /* operation flags */
        if (semop(semID, &semBuf, 1) == -1)
        {
            perror("semop in wait failed!");
            return false;
        }
        return true;
    }

    bool post(short value = 1)
    {
        if (semID == -1)
            return false;

        sembuf semBuf;
        semBuf.sem_num = 0;
        semBuf.sem_op = value;
        semBuf.sem_flg = semFlag;
        if (semop(semID, &semBuf, 1) == -1)
        {
            perror("semop in post failed!");
            return false;
        }
        return true;
    }

    int getValue()
    {
        return semctl(semID, 0, GETVAL);
    }

    bool destroy()
    {
        if (semID == -1)
            return false;
        if (semctl(semID, 0, IPC_RMID) == -1)
        {
            perror("semctl in destroy failed!");
            return false;
        }
        return true;
    }
};
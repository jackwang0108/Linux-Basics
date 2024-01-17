#include <iostream>

#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <Semephore.h>
#include <CircularQueue.h>

using std::cout;
using std::endl;

struct Person
{
    int no;
    char name[51];
};

std::ostream &operator<<(std::ostream &os, const Person &p)
{
    os << "Person No=" << p.no << ", name=" << p.name;
    return os;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./semephore-demo id name" << endl;
        return EXIT_FAILURE;
    }

    // 申请共享内存
    int shmID = shmget(0x5005, sizeof(Person), 0640 | IPC_CREAT);
    if (shmID == -1)
    {
        cout << "shmget failed!" << endl;
        return EXIT_FAILURE;
    }
    cout << "shmID=" << shmID << endl;

    // 连接到当前进程
    Person *ptr = (Person *)shmat(shmID, 0, 0);
    if (ptr == (void *)-1)
    {
        cout << "shmat failed!" << endl;
        return EXIT_FAILURE;
    }

    // 使用前先加锁
    Semaphore mutex;
    if (mutex.init(0x5005) == false)
    {
        perror("mutex init failed!");
        return EXIT_FAILURE;
    }

    mutex.wait();

    // 使用共享内存
    cout
        << "原值:" << *ptr << endl;
    ptr->no = atoi(argv[1]);
    strcpy(ptr->name, argv[2]);
    cout << "新值:" << *ptr << endl;
    sleep(10);

    // 使用结束后释放内存

    // 分离共享内存
    if (shmdt(ptr) == -1)
    {
        perror("shmdt failed!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
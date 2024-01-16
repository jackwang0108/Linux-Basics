#include <string>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using std::cout;
using std::endl;
using std::string;

// 多线程共享进程的地址空间，如果多个线程需要访问同一块内存，用全局变量就可以了。
// 在多进程中，由于虚拟内存, 每个进程的地址空间是独立的，不共享的，如果多个进程需要访问同一块内存，不能用全局变量，只能用共享内存。
// 共享内存（Shared Memory）允许多个进程（不要求进程之间有血缘关系）访问同一块内存空间，是多个进程之间共享和传递数据最高效的方式。进程可以将共享内存连接到它们自己的地址空间中，如果某个进程修改了共享内存中的数据，其它的进程读到的数据也会改变。
// 共享内存没有提供锁机制，也就是说，在某一个进程对共享内存进行读/写的时候，不会阻止其它进程对它的读/写。如果要对共享内存的读/写加锁，可以使用信号量。
// Linux中提供了一组函数用于操作共享内存。

// = 注意, 共享内存不要使用STL中的容器, 因为容器会在当前进程的堆中动态分配内存, 因此无法在多个进程间共用
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

// 一、shmget函数
// 该函数用于创建/获取共享内存。
//      int shmget(key_t key, size_t size, int shmflg);
//      key	 	共享内存的键值，是一个整数（typedef unsigned int key_t），一般采用十六进制，例如0x5005，不同共享内存的key不能相同。
//      size 	共享内存的大小，以字节为单位。
//      shmflg	共享内存的访问权限，与文件的权限一样，例如0666|IPC_CREAT，0666表示全部用户对它可读写，IPC_CREAT表示如果共享内存不存在，就创建它。
// 返回值：成功返回共享内存的id（一个非负的整数），失败返回-1（系统内存不足、没有权限）
// 用ipcs -m可以查看系统的共享内存，包括：键值（key），共享内存id（shmid），拥有者（owner），权限（perms），大小（bytes）。
// 用ipcrm -m 共享内存id可以手工删除共享内存，如下：

// 二、shmat函数
// 该函数用于把共享内存连接到当前进程的地址空间。
// void *shmat(int shmid, const void *shmaddr, int shmflg);
//      shmid		由shmget()函数返回的共享内存标识。
//      shmaddr 	指定共享内存连接到当前进程中的地址位置，通常填0，表示让系统来选择共享内存的地址。
//      shmflg		标志位，通常填0。
// 调用成功时返回共享内存起始地址，失败返回(void*)-1。

// 三、shmdt函数
// 该函数用于将共享内存从当前进程中分离，相当于shmat()函数的反操作。
//      int shmdt(const void *shmaddr);
//      shmaddr	shmat()函数返回的地址。
// 调用成功时返回0，失败时返回-1。

// 四、shmctl函数
// 该函数用于操作共享内存，最常用的操作是删除共享内存。
//      int shmctl(int shmid, int command, struct shmid_ds *buf);
//      shmid		shmget()函数返回的共享内存id。
//      command	操作共享内存的指令，如果要删除共享内存，填IPC_RMID。
//      buf			操作共享内存的数据结构的地址，如果要删除共享内存，填0。
// 调用成功时返回0，失败时返回-1。
// 注意，用root创建的共享内存，不管创建的权限是什么，普通用户无法删除。
void testShmat(int shmID)
{
}

int main(int argc, char *argv[])
{
    // 第一步: 申请共享内存
    int shmID = shmget(0x5005, sizeof(Person), 0640 | IPC_CREAT);
    if (shmID == -1)
        perror("Get Shared Memory Fail!");
    cout << "ShmID=" << shmID << endl;

    // 第二步: 链接到共享内存
    Person *personPtr = (Person *)shmat(shmID, 0, 0);
    if (personPtr == (void *)-1)
        perror("Shmat() failed!");

    // 第三步: 使用共享内存, 对共享内存进行读写
    cout << "原值: " << *personPtr << endl;
    personPtr->no = 51;
    strcpy(personPtr->name, "Jack Wang");
    cout << "新值: " << *personPtr << endl;

    // 第四步: 共享内存从当前进程中分离
    if (shmdt(personPtr) == -1)
        perror("Shmdt() failed!");

    // 第五步: 所有进程结束后, 删除共享内存
    if (shmctl(shmID, IPC_RMID, 0) == -1)
        perror("Remove Shared Memory Failed!");

    return 0;
}
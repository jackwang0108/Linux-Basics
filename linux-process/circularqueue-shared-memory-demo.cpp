#include <iostream>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "CircularQueue.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
	using ElemType = int;

	int shmID = shmget(0x5005, sizeof(CircularQueue<ElemType, 5>), 0640 | IPC_CREAT);
	if (shmID == -1)
		perror("shmget failed!");

	CircularQueue<ElemType, 5> *cq = (CircularQueue<ElemType, 5> *) shmat(shmID, 0, 0);
	if (cq == (void *) -1)
		perror("shmat failed!");

	// 初始化循环队列
	cq->init();

	ElemType val;
	for (val = 1; val <= 3; val++)
		cout << "元素 " << val << " 入队" << endl, cq->push(val);

	cout << "队列长度为: " << cq->size() << endl;
	cq->print();

	for (int i = 0; i < 2; i++) {
		val = cq->front();
		cq->pop();
		cout << "出队元素为: " << val << endl;
	}

	for (val = 11; val <= 15; val++)
		cout << "元素 " << val << " 入队" << endl, cq->push(val);
	cout << "队列长度为: " << cq->size() << endl;
	cq->print();

	shmdt(cq);

	return 0;
}
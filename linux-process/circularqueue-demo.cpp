#include <iostream>

#include "CircularQueue.h"

using std::cout;
using std::endl;

int main() {
	CircularQueue<int, 5> cq;

	// According to Girish Shetty:

	// There are many factors that decide the size of an object of a class in C++.

	// These factors are:
	//  - Size of all non-static data members
	//  - Order of data members
	//  - Byte alignment or byte padding
	//  - Size of its immediate base class
	//  - The existence of virtual function(s) (Dynamic polymorphism using virtual functions).
	//  - Compiler being used
	//  - Mode of inheritance (virtual inheritance)
	cout << sizeof(CircularQueue<int, 5>) << endl;
	cout << sizeof(bool) + sizeof(int) * 5 + sizeof(int) * 3 << endl;

	int val;

	cout << "元素 (1, 2, 3) 入队" << endl;
	for (val = 1; val <= 3; val++)
		cq.push(val);

	cout << "队列长度为: " << cq.size() << endl;

	cq.print();

	for (int i = 0; i < 2; i++) {
		val = cq.front();
		cq.pop();
		cout << "出队元素为: " << val << endl;
	}

	return 0;
}
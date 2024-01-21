#include <cstring>
#include <iostream>
#include <string>

#include <errno.h>
#include <sys/stat.h>

using std::cout;
using std::endl;

// 在C++程序中，如果调用了库函数，可以通过函数的返回值判断调用是否成功。
// 其实还有一个整型的全局变量errno，存放了函数调用过程中产生的错误代码。
// 如果调用库函数失败，可以通过errno的值来查找原因，这也是调试程序的一个重要方法。
// errno在<errno.h>中声明。
// 配合 strerror()和perror()两个库函数，可以查看出错的详细信息。

// 注意事项
//  1）调用库函数失败不一定会设置errno
//      并不是全部的库函数在调用失败时都会设置errno的值，以man手册为准（一般来说，不属于系统调用的函数不会设置errno，属于系统调用的函数才会设置errno）。什么是系统调用？
//  2）errno不能作为调用库函数失败的标志
//      errno的值只有在库函数调用发生错误时才会被设置，当库函数调用成功时，errno的值不会被修改，不会主动的置为 0。
//      在实际开发中，判断函数执行是否成功还得靠函数的返回值，只有在返回值是失败的情况下，才需要关注errno的值。

void testStrerror() {
	// strerror() 在<string.h>中声明，用于获取错误代码对应的详细信息。
	//      char *strerror(int errnum);                       	// 非线程安全。
	//      int strerror_r(int errnum, char *buf, size_t buflen);		// 线程安全。
	// gcc8.3.1一共有133个错误代码。
	for (int i = 0; i <= 133; i++)
		cout << "errno: " << i << ", error string: " << strerror(i) << endl;

	char dir[] = "Test";
	for (int i = 0; i < 2; i++)
		if (mkdir(dir, 0755) != 0)
			cout << "error: " << strerror(errno) << endl;

	// perror() 在<stdio.h>中声明，用于在控制台显示最近一次系统错误的详细信息，在实际开发中，服务程序在后台运行，通过控制台显示错误信息意义不大。（对调试程序略有帮助）
	// void perror(const char *s);
	char info[256];
	if (mkdir(dir, 0755) != 0) {
		sprintf(info, "Create dir %s failed", dir);
		perror(info);
	}
	cout << 1;
}

int main(int argc, char *argv[]) {
	testStrerror();
	return 0;
}
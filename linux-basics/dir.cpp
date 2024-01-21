#include <filesystem>
#include <iostream>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

using std::cout;
using std::endl;
namespace fs = std::filesystem;

void testGetCwd() {
	// 获取当前工作目录, 包含头文件：<unistd.h>
	//      char *getcwd(char *buf, size_t size);
	//      char *get_current_dir_name(void);
	char path1[256];// linux系统目录的最大长度是255
	getcwd(path1, 256);
	cout << "Current Working Dir: " << path1 << endl;

	// 使用这个函数要注意释放内存。malloc() free() new delete
	char *path2 = get_current_dir_name();
	cout << "Current Working Dir: " << path2 << endl;
	free(path2);
}

void testChdir() {
	// 包含头文件：<unistd.h>
	//      int chdir(const char *path);
	// 返回值：0-成功；其它-失败（目录不存在或没有权限）
	char cwd[256];
	getcwd(cwd, 256);
	cout << "Current Working Dir: " << cwd << endl;

	std::filesystem::path dir(cwd);
	cout << "Changing..." << endl;
	chdir(dir.parent_path().c_str());
	getcwd(cwd, 256);
	cout << "Current Working Dir: " << cwd << endl;
}

void testMkdir() {
	// 包含头文件：<sys/stat.h>
	//      int mkdir(const char *pathname, mode_t mode);
	//        - pathname-目录名。
	//        - mode-访问权限，如0755，不要省略前置的0。
	// 返回值：0-成功；其它-失败（上级目录不存在或没有权限）
	std::filesystem::path dir = std::filesystem::path(__FILE__).parent_path();
	chdir(dir.c_str());
	dir.append("Test");

	cout << "Dir to create: " << dir << endl;
	if (mkdir(dir.c_str(), 0755) == 0)
		cout << "创建成功" << endl;
	else
		cout << "创建失败, errno: " << errno << endl;
}

void testRmdir() {
	// 包含头文件： <unistd.h>
	//     int rmdir(const char *path);
	//     path-目录名。
	// 返回值：0-成功；其它-失败（目录不存在或没有权限）
	std::filesystem::path dir = std::filesystem::path(__FILE__).parent_path();
	dir.append("Test");
	cout << "Dir to remove: " << dir << endl;
	if (rmdir(dir.c_str()) == 0)
		cout << "删除成功" << endl;
	else
		cout << "删除失败" << endl;
}

void testReaddir() {
	// 文件存放在目录中，在处理文件之前，必须先知道目录中有哪些文件，所以要获取目录中文件的列表。
	// 1）包含头文件
	//      #include <dirent.h>
	// 2）相关的库函数
	//      步骤一：用opendir()函数打开目录。
	//           DIR *opendir(const char *pathname);
	//           成功-返回目录的地址，失败-返回空地址。
	//      步骤二：用readdir()函数循环的读取目录。
	//           struct dirent *readdir(DIR *dirp);
	//           成功-返回struct dirent结构体的地址，失败-返回空地址。
	//      步骤三：用closedir()关闭目录。
	//           int closedir(DIR *dirp);
	// 3）数据结构
	//      目录指针：
	//          DIR *目录指针变量名;
	//      每次调用readdir()，函数返回struct dirent的地址，存放了本次读取到的内容。
	//      struct dirent
	//      {
	//         long d_ino;                    			// inode number 索引节点号。
	//         off_t d_off;                   			// offset to this dirent 在目录文件中的偏移。
	//         unsigned short d_reclen;     		// length of this d_name 文件名长度。
	//         unsigned char d_type;         		// the type of d_name 文件类型。
	//         char d_name [NAME_MAX+1];    // file name文件名，最长255字符。
	//      };
	//      重点关注结构体的d_name和d_type成员。
	//      d_name-文件名或目录名。
	//      d_type-文件的类型，有多种取值，最重要的是8和4，8-常规文件（A regular file）；4-子目录（A directory），其它的暂时不关心。注意，d_name的数据类型是字符，不可直接显示。
	DIR *dir;
	fs::path d = fs::path(__FILE__).parent_path().parent_path();

	if ((dir = opendir(d.c_str())) == nullptr) {
		cout << "打开目录失败: " << d.c_str() << endl;
		return;
	}

	dirent *entryInfo = nullptr;
	while (1) {
		if ((entryInfo = readdir(dir)) == nullptr)
			break;
		cout << "文件名: " << entryInfo->d_name << ", 文件类型: " << (int) entryInfo->d_type << endl;
	}

	closedir(dir);
}

int main(int argc, char *argv[]) {
	testGetCwd();
	testChdir();
	testMkdir();
	testRmdir();
	testReaddir();
	return 0;
}
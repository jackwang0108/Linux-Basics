#include <string>
#include <cstring>
#include <iostream>
#include <filesystem>

#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace fs = std::filesystem;

using std::cout;
using std::endl;

void testAccess()
{
    // access()函数用于判断当前用户对目录或文件的存取权限。
    // 包含头文件：
    //      #include <unistd.h>
    // 函数声明：
    //      int access(const char *pathname, int mode);
    // 参数说明：
    //      pathname	目录或文件名。
    //      mode 		需要判断的存取权限。在头文件<unistd.h>中的预定义如下：
    //          #define R_OK 	4 	// 判断是否有读权限。
    //          #define W_OK	2 	// 判断是否有写权限。
    //          #define X_OK 	1 	// 判断是否有执行权限。
    //          #define F_OK 	0  	// 判断是否存在。
    // 返回值：
    // 当pathname满足mode权限返回0，不满足返回-1，errno被设置。
    // 在实际开发中，access()函数主要用于判断目录或文件是否存在。
    fs::path pathExist = fs::path(__FILE__).parent_path().parent_path().append("LICENSE");
    fs::path pathNoExist = fs::path(__FILE__).parent_path().parent_path().append("asdas");
    if (access(pathExist.c_str(), F_OK) == 0)
        cout << "File Exists: " << pathExist << endl;
    if (access(pathNoExist.c_str(), F_OK) != 0)
        perror(pathNoExist.c_str());
}

std::ostream &operator<<(std::ostream &os, const timespec &tm)
{
    os << "sec=" << tm.tv_sec << ", nsec=" << tm.tv_nsec;
    return os;
}

void testStat()
{
    // struct stat结构体用于存放目录或文件的详细信息，如下：
    //     struct stat
    //     {
    //       dev_t st_dev;   	// 文件的设备编号。
    //       ino_t st_ino;   		// 文件的i-node。
    //       mode_t st_mode; 	// 文件的类型和存取的权限。
    //       nlink_t st_nlink;   	// 连到该文件的硬连接数目，刚建立的文件值为1。
    //       uid_t st_uid;   		// 文件所有者的用户识别码。
    //       gid_t st_gid;   		// 文件所有者的组识别码。
    //       dev_t st_rdev;  	// 若此文件为设备文件，则为其设备编号。
    //       off_t st_size;  		// 文件的大小，以字节计算。
    //       size_t st_blksize;	// I/O 文件系统的I/O 缓冲区大小。
    //       size_t st_blocks;  	// 占用文件区块的个数。
    //       time_t st_atime;  	// 文件最近一次被存取或被执行的时间，在用mknod、 utime、read、write 与tructate 时改变。
    //       time_t st_mtime;  	// 文件最后一次被修改的时间，在用mknod、 utime 和write 时才会改变。
    //       time_t st_ctime;  	// 最近一次被更改的时间，在文件所有者、组、 权限被更改时更新。
    //     };
    // struct stat结构体的成员变量比较多，重点关注st_mode、st_size和st_mtime成员。注意：st_mtime是一个整数表示的时间，需要程序员自己写代码转换格式。
    // st_mode成员的取值很多，用以下两个宏来判断：
    //     S_ISREG(st_mode)  // 是否为普通文件，如果是，返回真。 
    //     S_ISDIR(st_mode)  // 是否为目录，如果是，返回真。

    // stat()库函数
    // 包含头文件：
    //      #include <sys/stat.h>
    // 函数声明：
    //      int stat(const char *path, struct stat *buf);
    // stat()函数获取path参数指定目录或文件的详细信息，保存到buf结构体中。
    // 返回值：0-成功，-1-失败，errno被设置。
    struct stat st;
    fs::path pathExist = fs::path(__FILE__).parent_path().parent_path().append("LICENSE");
    if (stat(pathExist.c_str(), &st) != 0)
        perror(pathExist.c_str());

    if (S_ISREG(st.st_mode))
        cout << pathExist << "是一个文件(mtime=" << st.st_mtim << ",size=" << st.st_size << ")" << endl;
    if (S_ISDIR(st.st_mode))
        cout << pathExist << "是一个目录(mtime=" << st.st_mtim << ",size=" << st.st_size << ")" << endl;
}

// utime()函数用于修改目录或文件的时间。
// 包含头文件：
// #include <sys/types.h>
// #include <utime.h>
// 函数声明：
// int utime(const char *filename, const struct utimbuf *times);
// utime()函数用来修改参数filename的st_atime和st_mtime。如果参数times为空地址，则设置为当前时间。结构utimbuf 声明如下：
// struct utimbuf
// {
//   time_t actime;
//   time_t modtime;
// };
// 返回值：0-成功，-1-失败，errno被设置。
// ! 实际开发中这个函数不能直接用, 需要二次封装
void testUtime()
{
    struct stat st;
    fs::path pathExist = fs::path(__FILE__).parent_path().parent_path().append("LICENSE");
    if (stat(pathExist.c_str(), &st) != 0)
        perror(pathExist.c_str());

    tm tm;
    localtime_r(&st.st_mtim.tv_sec, &tm);
    cout << "Last mtime: " << st.st_mtim.tv_sec << ", " << tm.tm_year + 1900 << "-" << tm.tm_mon << "-" << tm.tm_yday << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << endl;

    utimbuf tb{time(nullptr), time(nullptr)};
    utime(pathExist.c_str(), &tb);

    if (stat(pathExist.c_str(), &st) != 0)
        perror(pathExist.c_str());
    localtime_r(&st.st_mtim.tv_sec, &tm);
    cout << "Last mtime: " << st.st_mtim.tv_sec << ", " << tm.tm_year + 1900 << "-" << tm.tm_mon << "-" << tm.tm_yday << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << endl;
}

// rename()函数用于重命名目录或文件，相当于操作系统的mv命令。
// 包含头文件：
// #include <stdio.h>
// 函数声明：
// int rename(const char *oldpath, const char *newpath);
// 参数说明：
// oldpath 	原目录或文件名。
// newpath 	目标目录或文件名。
// 返回值：0-成功，-1-失败，errno被设置。
void testRename()
{
    fs::path pathExist = fs::path(__FILE__).parent_path().parent_path().append("Test");
    mkdir(pathExist.c_str(), 0755);
    if (rename(pathExist.c_str(), pathExist.parent_path().append("temp").c_str()) != 0)
        perror("rename failed");
}

// remove()函数用于删除目录或文件，相当于操作系统的rm命令。
// 包含头文件：
// #include <stdio.h>
// 函数声明：
// int remove(const char *pathname);
// 参数说明：
// pathname 待删除的目录或文件名。
// 返回值：0-成功，-1-失败，errno被设置。
void testRemove()
{
    fs::path pathExist = fs::path(__FILE__).parent_path().parent_path().append("temp");
    if (remove(pathExist.c_str()) != 0)
        perror(pathExist.c_str());
}

int main(int argc, char *argv[])
{
    testAccess();
    testStat();
    testUtime();
    testRename();
    testRemove();
    return 0;
}
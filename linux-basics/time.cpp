#include <ctime>
#include <string>
#include <iostream>

#include <unistd.h>
#include <sys/time.h>

using std::cout;
using std::endl;
using std::string;

// UNIX操作系统根据计算机产生的年代把1970年1月1日作为UNIX的纪元时间.
// 1970年1月1日是时间的中间点，将从1970年1月1日起经过的秒数用一个整数存放

// time_t用于表示时间类型，它是一个long类型的别名，在<time.h>文件中定义，表示从1970年1月1日0时0分0秒到现在的秒数。
// typedef long time_t;

void testTime_t()
{
    // time()库函数用于获取操作系统的当前时间。包含头文件：<time.h>
    // 声明：
    //     time_t time(time_t *tloc);
    // 有两种调用方法：
    //     time_t now=time(0);    		// 将空地址传递给time()函数，并将time()返回值赋给变量now。
    // 或
    //     time_t now;
    //     time(&now);                  // 将变量now的地址作为参数传递给time()函数。
    time_t before = time(0);
    cout << "Before: " << before << endl;
    for (int i = 0; i < INT32_MAX / 8; i++)
        ;
    time_t after;
    time(&after);
    cout << "After: " << after << endl;
}

string tmToString(const tm &tm)
{
    return std::move(
        std::to_string(tm.tm_year + 1900) + '-' +
        std::to_string(tm.tm_mon + 1) + '-' +
        std::to_string(tm.tm_mday) + " " +
        std::to_string(tm.tm_hour) + ":" +
        std::to_string(tm.tm_min) + ":" +
        std::to_string(tm.tm_sec));
}

// time_t是一个长整数，不符合人类的使用习惯，人类更习惯: 2022-10-01 15:30:25   Oct 1,2022 15:30:25
// 因此需要把time_t需要转换成tm结构体，tm结构体在<time.h>中声明，如下：
// struct tm
// {
//   int tm_year;	// 年份：其值等于实际年份减去1900
//   int tm_mon;	// 月份：取值区间为[0,11]，其中0代表一月，11代表12月
//   int tm_mday;	// 日期：一个月中的日期，取值区间为[1,31]
//   int tm_hour; 	// 时：取值区间为[0,23]
//   int tm_min;	// 分：取值区间为[0,59]
//   int tm_sec;     	// 秒：取值区间为[0,59]
//   int tm_wday;	// 星期：取值区间为[0,6]，其中0代表星期天，6代表星期六
//   int tm_yday;	// 从每年的1月1日开始算起的天数：取值区间为[0,365]
//   int tm_isdst;   // 夏令时标识符，该字段意义不大
// };
void testTm()
{
    // localtime()函数用于把time_t表示的时间转换为tm结构体表示的时间。
    // localtime()函数不是线程安全的，localtime_r()是线程安全的。
    time_t now = time(nullptr);
    cout << "Now: " << now << endl;

    tm tmNow;
    localtime_r(&now, &tmNow);
    string timeStr = std::to_string(tmNow.tm_year + 1900) + '-' +
                     std::to_string(tmNow.tm_mon + 1) + '-' +
                     std::to_string(tmNow.tm_mday) + " " +
                     std::to_string(tmNow.tm_hour) + ":" +
                     std::to_string(tmNow.tm_min) + ":" +
                     std::to_string(tmNow.tm_sec);
    cout << "Now: " << timeStr << endl;
}

void testMktime()
{
    // mktime()函数的功能与localtime()函数相反，用于把tm结构体时间转换为time_t时间。包含头文件：<time.h>
    // 函数声明：
    //     time_t mktime(struct tm *tm);
    // 该函数主要用于时间的运算，例如：把2022-03-01 00:00:25加30分钟。
    // 思路：
    //     1）解析字符串格式的时间，转换成tm结构体；
    //     2）用mktime()函数把tm结构体转换成time_t时间；
    //     3）把time_t时间加30*60秒；
    //     4）用localtime_r()函数把time_t时间转换成tm结构体；
    //     5）把tm结构体转换成字符串。
    time_t now = time(nullptr);
    tm tmNow;
    localtime_r(&now, &tmNow);
    cout << "Current time: " << tmToString(tmNow) << ", " << now << endl;
    tmNow.tm_min += 30;
    cout << "After 30 mins: " << tmToString(tmNow) << ", " << mktime(&tmNow) << endl;
}

void testGettimeofday()
{
    // gettimeofday()库函数用于获取1970年1月1日到现在的秒和当前秒中已逝去的微秒数，可以用于程序的计时。 包含头文件：<sys/time.h>
    // 1 sec = 10^6 usec
    // 函数声明：
    //     int gettimeofday(struct timeval *tv, struct timezone *tz);
    // struct timeval {
    //   time_t      tv_sec;    	/* 1970-1-1到现在的秒数 */
    //   suseconds_t tv_usec;   	/* 当前秒中，已逝去的微秒数 */
    // };
    // struct timezone {         /* 在实际开发中，派不上用场 */
    //   int tz_minuteswest;   	/* minutes west of Greenwich */
    //   int tz_dsttime;         	/* type of DST correction */
    // };
    timeval start, end;
    gettimeofday(&start, nullptr);
    for (int i = 0; i < INT32_MAX / 4; i++)
        ;
    gettimeofday(&end, 0);

    timeval spend;
    spend.tv_sec = end.tv_sec - start.tv_sec;
    spend.tv_usec = end.tv_usec - start.tv_usec;
    cout << "Time used: " << spend.tv_sec << " secs, " << spend.tv_usec << " usecs" << endl;
}

void testSleep()
{
    // 如果需要把程序挂起一段时间，可以使用sleep()和usleep()两个库函数。
    // 包含头文件：<unistd.h>
    timeval start, end;
    gettimeofday(&start, nullptr);
    sleep(2);
    usleep(100000);
    gettimeofday(&end, 0);
    timeval spend;
    spend.tv_sec = end.tv_sec - start.tv_sec;
    spend.tv_usec = end.tv_usec - start.tv_usec;
    if (spend.tv_usec < 0)
    {
        spend.tv_sec--;
        spend.tv_usec += 1000000;
    }
    cout << "Time sleeped: " << spend.tv_sec << " secs, " << spend.tv_usec << " usecs" << endl;
}

int main(int argc, char *argv[])
{
    testTime_t();
    testTm();
    testMktime();
    testGettimeofday();
    testSleep();
    return 0;
}
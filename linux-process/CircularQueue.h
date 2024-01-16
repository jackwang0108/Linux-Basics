#pragma once
#include <cstring>
#include <iostream>

template <class T, int maxLength>
class CircularQueue
{
private:
    bool initialized;
    T data[maxLength];
    int head;
    int tail;
    int length;

    // 禁止拷贝构造函数和赋值运算符重载
    CircularQueue(const CircularQueue &) = delete;
    CircularQueue &operator=(const CircularQueue &) = delete;

public:
    CircularQueue() : initialized(false) { init(); }
    ~CircularQueue() {}

    void init()
    {
        if (initialized)
            return;
        head = 0;
        tail = 0;
        length = 0;
        memset(data, 0, sizeof(data));
        initialized = true;
    }

    int size() const { return length; };
    bool full() const { return (tail + 1) % maxLength == head; };
    bool empty() const { return length == 0; };
    T &front() { return data[head]; }
    const T &front() const { return data[head]; }
    T &back() { return data[tail]; }
    const T &back() const { return data[tail]; }
    bool pop()
    {
        if (empty())
            return false;
        head = (head + 1) % maxLength;
        length--;
        return true;
    }
    bool push(const T &val)
    {
        if (full())
        {
            std::cout << "队列已满, 入队失败!" << std::endl;
            return false;
        }

        tail = (tail + 1) % maxLength;
        data[tail] = val;
        length++;

        return true;
    }
    void print() const
    {
        for (int i = 0; i < size(); i++)
            std::cout << "data[" << (head + i) % maxLength << "]=" << data[(head + i) % maxLength] << ", ";
        std::cout << std::endl;
    }
};
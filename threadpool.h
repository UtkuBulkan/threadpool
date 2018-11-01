#include <iostream>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <map>
#include <algorithm>
#include <utility>
#include <thread>
#include <typeindex>
#include <queue>

class Taskmap {
public:
    void insert_task(int priority, std::function<void()> func);
    bool empty();
    std::function<void()> pull_task();
private:
    std::multimap<int, std::function<void()> > m;
    std::mutex mut;
};

class Threadpool {
public:
    Threadpool(int max_worker);
    ~Threadpool();
    void end_pool();
    template<class F, class... Args> void queue_task(int pri, F&& f, Args&&... args);
private:
    std::mutex mut;
    std::condition_variable cv;
    std::deque<std::thread> thread_deque;
    Taskmap tm;
    bool quit;
};


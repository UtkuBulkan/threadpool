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

#include "threadpool.h"

void Taskmap::insert_task(int priority, std::function<void()> func) 
{
    std::lock_guard<std::mutex> guard(mut);
    m.insert(std::make_pair(priority, func));
}

bool Taskmap::empty() 
{
    std::lock_guard<std::mutex> guard(mut);
    return m.empty();
}

std::function<void()> Taskmap::pull_task()
{
    std::lock_guard<std::mutex> guard(mut);
    auto it = m.rbegin();
    if (it == m.rend()) return nullptr;
    auto task = it->second;
    m.erase(std::next(it).base());
    return task ;
}

Threadpool::Threadpool(int max_worker) : quit(false)
{
    for(int i=0; i<max_worker;i++) {
	thread_deque.emplace_back(
			    [this, i]
			    {
				while(1)
				{
				    std::function<void()> task;
				    {
					std::unique_lock<std::mutex> lock(this->mut);
					this->cv.wait(lock,[this]{ return this->quit || !this->tm.empty(); });
					if(this->quit && this->tm.empty()) return;
					task = this->tm.pull_task();
				    }
				    task();
				}
			    }
	    );
	}
}
Threadpool::~Threadpool() 
{
    quit = true;
    cv.notify_all();
    while(!thread_deque.empty()) {
	(thread_deque.front()).join();
	thread_deque.pop_front();
    }
}
void Threadpool::end_pool()
{
    std::lock_guard<std::mutex> guard(mut);
    quit = true;
    cv.notify_all();
}
template<class F, class... Args> void Threadpool::queue_task(int pri, F&& f, Args&&... args)
{
    {
	std::lock_guard<std::mutex> guard(mut);
	tm.insert_task(pri, std::function<void()>(f));
    }
    cv.notify_one();
}

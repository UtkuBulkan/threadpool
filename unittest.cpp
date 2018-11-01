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

void task()
{
    usleep(1);
}

class Unittest {
public:
    Unittest(int nboftask, int nbofworker) : max_task(nboftask), max_worker(nbofworker)
    {
	Threadpool fl(max_worker);
	std::cout << "UnitTest; Nb of workers : " << max_worker << ", Nb of tasks : " << max_task << "\n";

	for(int i=0; i<max_task; i++) {
	    fl.queue_task(1+std::rand()%99, task);
	}
    }
private:
    int max_worker, max_task;
};

int main()
{
    Unittest u1(100, 3);
    Unittest u2(1000, 10);
    Unittest u3(100000, 100);
    Unittest u4(1, 10);
    Unittest u5(0, 3);
}

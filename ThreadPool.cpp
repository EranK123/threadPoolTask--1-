// #include <thread>
#include "codec.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <iostream>
// #include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>

class ThreadPool
{
public:
    ThreadPool(long num_threads) : stop(false)
    {
        threads.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i)
        {
            pthread_t thread_id;
            pthread_create(&thread_id, nullptr, worker_thread, this);
            threads.push_back(thread_id);
        }
    }

    template <class F, class... Args>
    void enqueue(F &&f, Args &&...args)
    {
        auto task = std::make_shared<std::function<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace([task]()
                          { (*task)(); });
        }
        condition.notify_one();
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (pthread_t thread_id : threads)
            pthread_join(thread_id, nullptr);
    }

private:
    static void *worker_thread(void *arg)
    {
        auto *pool = static_cast<ThreadPool *>(arg);
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(pool->queue_mutex);
                pool->condition.wait(lock, [pool]
                                     { return pool->stop || !pool->tasks.empty(); });
                if (pool->stop && pool->tasks.empty())
                    return nullptr;
                task = std::move(pool->tasks.front());
                pool->tasks.pop();
            }
            task();
        }
    }

    std::vector<pthread_t> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};


#include <iostream>
#include <chrono>
#include <unistd.h>

void print_hello(int id) {
    std::cout << "Hello from thread " << id << std::endl;
    usleep(1000000);
}

int main() {
    ThreadPool pool(4);

    for (int i = 0; i < 8; ++i) {
        pool.enqueue(print_hello, i);
    }

    usleep(5000000);

    return 0;
}


// void compute(int num)
// {
//     std::cout << "Computing " << num << " on thread " << std::endl;
// }

// int main()
// {
//     long num_threads = sysconf(_SC_NPROCESSORS_ONLN);
//     ThreadPool pool(num_threads);
//     for (int i = 0; i < num_threads * 2; ++i)
//     {
//         pool.enqueue(compute, i);
//     }
//     return 0;
// }

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

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

void (*enc)(char *s, int key);
void (*dec)(char *s, int key);

struct task{
    int id;
    int key;
    char* data;
};

std::vector<pthread_t> threads;
std::queue<struct task> tasks;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_two = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
bool stop;
    // ThreadPool(long num_threads) : stop(false)
    // {    
    // template <class F, class... Args>
    // void enqueue(F &&f, Args &&...args)
    // {
    //     Task task = std::make_shared<std::function<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    //     {
    //         std::unique_lock<std::mutex> lock(queue_mutex);
    //         tasks.emplace([task]()
    //                       { (*task)(); });
    //     }
    //     condition.notify_one();
    // }

    // ~ThreadPool()
    // {
    //     {
    //         // std::unique_lock<std::mutex> lock(queue_mutex);
    //         pthread_mutex_lock(&mutex);
    //         stop = true;
    //     }
    //     pthread_cond_broadcast(&cond1);
    //     for (pthread_t thread_id : threads)
    //         pthread_join(thread_id, nullptr);
    // }

    void *worker_thread(void *arg)
    {
        // struct task* task = static_cast<struct task *>(arg);
        // while (true)
        // {
        //     task = NULL;
            // {
                // std::unique_lock<std::mutex> lock(pool->queue_mutex);
                // pthread_mutex_lock(&threads->);
                // condition.wait(lock, [pool]
                //                      { return pool->stop || !pool->tasks.empty(); });
            //     if (stop && pool->tasks.empty())
            //         return nullptr;
            //     task = std::move(pool->tasks.front());
            //     pool->tasks.pop();
            // }
            // task();
        // }
    }
    // void *handle_threads(void *arg){
        // while(true){
        //     pthread_mutex_lock(&mutex);

        //     while(!tasks.size() && !threads.size())
        //         pthread_cond_wait(&cond1, &mutex);
        //     struct task *task = (struct task*)malloc(sizeof(struct task));
        //     *task = tasks.front();
        //     threads.erase(threads.begin());
        //     tasks.pop();
        //     pthread_mutex_unlock(&mutex);
        // }
    // }
// #include "ThreadPool.cpp"
// void print_hello(int id) {
//     std::cout << "Hello from thread " << id << std::endl;
//     usleep(1000000);
// }

int main(int argc, char *argv[]) {
    // int num_of_threads = get_nprocs();
    
    int num_of_threads = sysconf(_SC_NPROCESSORS_ONLN);
    // ThreadPool pool(num_of_threads);
    threads.reserve(num_of_threads);
    for (size_t i = 0; i < num_of_threads; ++i)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, worker_thread, NULL);
        threads.push_back(thread_id);
    }

    void* handle = dlopen("./libCodec.so", RTLD_LAZY | RTLD_GLOBAL);
    if(!handle){
        fprintf(stderr, "dlerror: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    enc = (void (*)(char *s, int key))dlsym(handle,"encrypt");
    dec = (void (*)(char *s, int key))dlsym(handle,"decrypt");


    if (argc != 2)
	{
	    printf("usage: key < file \n");
	    printf("!! data more than 1024 char will be ignored !!\n");
	    return 0;
	}

	int key = atoi(argv[1]);
	printf("key is %i \n",key);
    int id = 0;
	char c;
	int counter = 0;
	int dest_size = 1024;
	char data[dest_size]; 
    memset(data, '\0', sizeof(data));
    struct task *task = (struct task*)malloc(sizeof(struct task));

	while ((c = getchar()) != EOF){
	  data[counter] = c;
	  counter++;

	  if (counter == 1024){
        task->data = data;
        task->key = key;
        task->id = id;
        pthread_mutex_lock(&mutex);
        tasks.push(*task);
        pthread_mutex_unlock(&mutex);
		// encrypt(data,key);
		printf("encripted data: %s\n",data);
		counter = 0;
	  }
	}
	
	if (counter > 0)
	{
		char lastData[counter];
		lastData[0] = '\0';
		strncat(lastData, data, counter);
        (*task).data = lastData;
        (*task).key = key;
        (*task).id = id;
        // tasks.push(*task);
        pthread_mutex_lock(&mutex);
        tasks.push(*task);
        pthread_mutex_unlock(&mutex);
		// encrypt(lastData,key);
		printf("encripted data:\n %s\n",lastData);
	}
    id++;

    while(true){
        pthread_mutex_lock(&mutex);

        while(!tasks.size() && !threads.size())
            pthread_cond_wait(&cond1, &mutex);
        struct task *task = (struct task*)malloc(sizeof(struct task));
        *task = tasks.front();
        pthread_t t = threads.at(0);
        threads.erase(threads.begin());
        threads.push_back(t);
        tasks.pop();
        pthread_mutex_unlock(&mutex);
    }

    if(!strcmp((char*)argv[2], "-e")){
        enc(task->data, task->key);
    }else{
        dec(task->data, task->key);    
    }
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

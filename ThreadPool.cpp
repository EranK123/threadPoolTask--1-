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
#include <semaphore.h>
#include <sys/sysinfo.h>

void (*encrypt_func)(char *s, int key);
void (*decrypt_func)(char *s, int key);

struct task{
    int id;
    int key;
    char data[1024];
    char type[2];
};

std::vector<pthread_t> threads;
std::queue<struct task> tasks;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_two = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;
sem_t job_queue_count;
   
    void *worker_thread(void *arg)
    {
        // printf("arg %p \n", arg);
        struct task *next_task;
        // std::cout<<"69";
        // exit(1);
        while(1){
            printf("45\n");
            // pthread_mutex_lock(&mutex_two);
            pthread_mutex_lock(&mutex);
            // sem_wait(&job_queue_count);'
            printf("tasks.front.data %s \n", tasks.front().data);
            printf("49\n");
            while (tasks.empty())
            {
                printf("53\n");
                pthread_cond_wait(&cond, &mutex);
            }
            
            *next_task = tasks.front();
            printf("len %d \n", next_task->id);
            std::cout<<"data"<<next_task->data;
            // printf("data %s \n", next_task->data);
            printf("\nkey %d \n", next_task->key);
            printf("next_task->type %d\n ", strcmp(next_task->type, "-e"));
            // if (next_task->data[0] == '-1')
            // {
            //     exit(1);
            // }
            pthread_mutex_unlock(&mutex);

            // pthread_mutex_unlock(&mutex_two);
            printf("next_task->type!! %s\n ", next_task->type);
            if(!strcmp(next_task->type, "-e")){
                printf("71!!!!!!!!!!!!!!!!!! \n");
                encrypt_func(next_task->data, next_task->key);
            }else{
                decrypt_func(next_task->data, next_task->key);    
            }
            tasks.pop();

        }
        // return NULL;
    }

int main(int argc, char *argv[]) {
    // int num_of_threads = get_nprocs();
    // std::cout<<"106";
    int num_of_threads = get_nprocs_conf();//sysconf(_SC_NPROCESSORS_ONLN);
    // ThreadPool pool(num_of_threads);
    threads.reserve(num_of_threads);
    for (size_t i = 0; i < num_of_threads; ++i)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, worker_thread, NULL);
        threads.push_back(thread_id);
    //     // printf("thread_id %ld\n", thread_id);
    }

    // std::cout<<"116";
    void* handle = dlopen("./libCodec.so", RTLD_LAZY | RTLD_GLOBAL);
    if(!handle){
        fprintf(stderr, "dlerror: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    
    encrypt_func = (void (*)(char *s, int key))dlsym(handle,"encrypt");
    decrypt_func = (void (*)(char *s, int key))dlsym(handle,"decrypt");

    char *type1 = "-e";
    char *type2 = "-d";
    // if (argc != 2)
	// {
	//     printf("usage: key < file \n");
	//     printf("!! data more than 1024 char will be ignored !!\n");
	//     return 0;
	// }
	int key = atoi(argv[1]);
	// printf("key is %i \n",key);
    int id = 0;
	char c;
	int counter = 0;
	int dest_size = 1024;
	char data[dest_size]; 
    char type[2];
    strcpy(type, argv[2]);
    // printf("type %s \n", type);
    // memset(data, '\0', sizeof(data));
    // memset(task->data, '\0', 1024);
    // sem_init(&job_queue_count , 0 , 0);
    // if(!strcmp(argv[2], "-e")){
    //     strcat(task->type, type1);
    // }else{
    //     strcat(task->type, type2);  
    // }

	while ((c = getchar()) != EOF){
       data[counter] = c;
       counter++;

        if (counter == 1024){
            printf("135!!!");
            struct task *task = (struct task*)malloc(sizeof(struct task));
            pthread_mutex_lock(&mutex);
            strcat(task->data, data);
            strcat(task->type, type);
            task->key = key;
            task->id = id;
            tasks.push(*task);
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
		    counter = 0;
            id++;
        }
    }
    printf("counter %d \n", counter);
    
    if (counter > 0){


        // pthread_mutex_lock(&mutex);
        char lastData[counter];
        lastData[0] = '\0';
        // printf("data! %s\n", data);
        // printf("last_data! %s\n", lastData);
        strncat(lastData, data, counter);
        // printf("last_data!!! %s\n", lastData);
        struct task *task2 = (struct task*)malloc(sizeof(struct task));
        memset(task2->data, '\0', 1024);
        strcpy(task2->data, lastData);
        // strncat(data, '\0', strlen(data));
        strcpy(task2->type, type);
        (*task2).key = key;
        (*task2).id = id;
        tasks.push(*task2);
        // pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
        // sem_post(&job_queue_count);

        printf("tasks[0] %s\n", tasks.front().data);
        printf("tasks[0] %ld\n", strlen(tasks.front().data));
        // exit(1);
    }
    // while(1){ continue; }

    return 0;
}

// #include "codec.h"
// #include <queue>
// #include <functional>
// #include <vector>
// #include <iostream>
// #include <unistd.h>
// #include <pthread.h>

// #include <iostream>
// #include <chrono>
// #include <unistd.h>
// #include <dlfcn.h>
// #include <stdio.h>
// #include <string.h>
// #include <semaphore.h>
// #include <sys/sysinfo.h>
// #include <string>


#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <dlfcn.h>
// using namespace std;
void (*encrypt_func)(char *s, int key);
void (*decrypt_func)(char *s, int key);

struct task{
    int id;
    int key;
    // string data;
    char data[1024];
    char type[2];
};

std::vector<pthread_t> threads;
std::queue<struct task> tasks;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_index = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;
pthread_cond_t cond_write;
// sem_t job_queue_count;
// int index_w;
int count_tasks;
int size_tasks;
    void *worker_thread(void *arg)
    {
        // printf("arg %p \n", arg);
        // struct task *next_task =(struct task*)malloc(sizeof(struct task));
        // std::cout<<"69";
        // exit(1);
        // printf("tasks_size %d ", tasks.empty());
        while(count_tasks < size_tasks){
            // printf("45\n");
            // pthread_mutex_lock(&mutex_two);
            pthread_mutex_lock(&mutex1);
            struct task *next_task =(struct task*)malloc(sizeof(struct task));
            // sem_wait(&job_queue_count);'
            // printf("tasks.front.data %s \n", tasks.front().data);
            // printf("49\n");
            // while (tasks.empty())
            // {
            //     // printf("53\n");
            //     pthread_cond_wait(&cond, &mutex1);
            // }
            
            *next_task = tasks.front();
            // printf("len %d \n", next_task->id);
            //     fflush(stdout);

            // std::cout<<"data"<<next_task->data;
            // printf("data %s \n", next_task->data);
            //     fflush(stdout);

            // printf("\nkey %d \n", next_task->key);
            //     fflush(stdout);

            // printf("next_task->type %d\n ", strcmp(next_task->type, "-e"));
            //     fflush(stdout);

            // if (next_task->data[0] == '-1')
            // {
            //     exit(1);
            // }
            // pthread_mutex_unlock(&mutex);

            // pthread_mutex_unlock(&mutex_two);
            // printf("next_task->type!! %s\n ", next_task->type);
            //     fflush(stdout);
            count_tasks++;
            // char data[1024];
            // strcpy(data, next_task->data.c_str()); 
            // strcpy(data, next_task->data); 
            // string zero = 
            // strcat(data, "\0");
            int key = next_task->key;
            // char *data = (char*)(malloc(sizeof(char)*strlen(next_task->data)));
            // strcpy(data, next_task->data);
            // exit(1);
            // printf("%s", next_task->data);
            if(!strcmp(next_task->type, "-e")){

                //  strcat(next_task->data, "\0");
                     // std::cout<<"data"<<next_task->data;
            // printf("data7111 %s \n", next_task->data);
            //     fflush(stdout);
                // char data[1024];
                // strcpy(data, next_task->data.c_str()); 

                encrypt_func(next_task->data, next_task->key);
                printf("%s", next_task->data);
                fflush(stdout);
                // encrypt_func(data, next_task->key);
            //     printf("1111");
            //     fflush(stdout);
            }else if(!strcmp(next_task->type, "-d")){
                decrypt_func(next_task->data, next_task->key);    

                // decrypt_func(data, next_task->key);
                printf("%s", next_task->data);
                fflush(stdout);
            }
            // printf("%s", next_task->data);
            // fflush(stdout);
            tasks.pop();
            pthread_mutex_unlock(&mutex1);


        }
        // pthread_exit(NULL);
        return NULL;
    }

int main(int argc, char *argv[]) {
    // int num_of_threads = get_nprocs();
    // std::cout<<"106";
    int num_of_threads = 2;//sysconf(_SC_NPROCESSORS_ONLN);//get_nprocs_conf();
    // printf("num_of_threads %d \n", num_of_threads);
    // ThreadPool pool(num_of_threads);

    // std::cout<<"116";
    void* handle = dlopen("./libCodec.so", RTLD_LAZY | RTLD_GLOBAL);
    if(!handle){
        // fprintf(stderr, "dlerror: %s\n", dlerror());
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
    // index_w = 0;
    count_tasks = 0;
    // printf("type %s \n", type);

    // memset(task->data, '\0', 1024);
    // sem_init(&job_queue_count , 0 , 0);
    // if(!strcmp(argv[2], "-e")){
    //     strcat(task->type, type1);
    // }else{
    //     strcat(task->type, type2);  
    // }
    // memset(data, '\0', 1024);
	while ((c = getchar()) != EOF){
       data[counter] = c;
       counter++;

        if (counter == 1024){
            // printf("135!!!");
            struct task *task = (struct task*)malloc(sizeof(struct task));
            // bzero(task->data, 1024);
            // memset(task->data,'\0', 1024);
            // pthread_mutex_lock(&mutex1);
            strcpy(task->data, data);
            // task->data = data;
            // strcat(task->data, "\0");
            strcpy(task->type, type);
            task->key = key;
            task->id = id;
            tasks.push(*task);
            size_tasks++;
            // pthread_mutex_unlock(&mutex);
            // pthread_cond_broadcast(&cond);
		    counter = 0;
            id++;
            // memset(data, '\0', 1024);
            // pthread_mutex_unlock(&mutex1);
            // while (index_w < task->id){
            //     pthread_cond_wait(&cond_write, &mutex_index);
            // }   
            // index_w++;
            // pthread_cond_broadcast(&cond_write);
            // pthread_cond_broadcast(&cond);
            // printf("data %s \n", task->data);
        }
    }
    // printf("counter %d \n", counter);
    // fflush(stdout);
    if (counter > 0){


        // pthread_mutex_lock(&mutex1);
        char lastData[counter];
        lastData[0] = '\0';
        // printf("data! %s\n", data);
        // printf("last_data! %s\n", lastData);
        strncat(lastData, data, counter);
        // printf("last_data!!! %s\n", lastData);
        struct task *task2 = (struct task*)malloc(sizeof(struct task));
        memset(task2->data, '\0', 1024);
        strcpy(task2->data, lastData);
        // task2->data = lastData;
        // printf("data task2%s \n", task2->data);
            //     fflush(stdout);
        // strncat(data, '\0', strlen(data));
        strcpy(task2->type, type);
        task2->key = key;
        task2->id = id;
        tasks.push(*task2);
        // size_tasks++;
        // pthread_mutex_unlock(&mutex);
        // pthread_cond_broadcast(&cond);
        // sem_post(&job_queue_count);

        // printf("tasks[0] %s\n", tasks.front().data);
        //                 fflush(stdout);

        // printf("tasks[0] %ld\n", strlen(tasks.front().data));
        // exit(1);
    }
    
    // threads.reserve(num_of_threads);
    // printf("num of threads %d \n", num_of_threads);
    // printf("tasks.size %d \n", tasks.size());
    for (int i = 0; i < num_of_threads && tasks.size(); ++i)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, worker_thread, NULL);
        threads.push_back(thread_id);
    //     // printf("thread_id %ld\n", thread_id);
    }

    for (int i = 0; i < num_of_threads; i++)
    {
        pthread_join(threads.at(i), NULL);
    }
    // exit(1);
    // while(1){ continue; }

    return 0;
}

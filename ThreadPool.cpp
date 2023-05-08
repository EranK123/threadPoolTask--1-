#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include <unistd.h>
#include <unordered_map>

using namespace std;
void (*encrypt_func)(char *s, int key);
void (*decrypt_func)(char *s, int key);

vector<pthread_t> threads;
vector<string> tasks;
unordered_map<int, string> all_data;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int count_tasks;
int key;
char type[2];

    void *worker_thread(void *arg)
    {
          while(count_tasks < tasks.size()){
            pthread_mutex_lock(&mutex1);
            int idx = count_tasks;
            char data[1024]; 
            strcpy(data, tasks.at(idx).c_str()); 
            count_tasks++;


            pthread_mutex_unlock(&mutex1);
            if(!strcmp(type, "-e")){
                encrypt_func(data, key);            
            }else if(!strcmp(type, "-d")){
                decrypt_func(data, key);    

            }
            string txt(data);
            all_data[idx] = txt; //string(data)
            
        }
        pthread_exit(NULL);
        return NULL;
    }

int main(int argc, char *argv[]) {
                                                    
    int num_of_threads = get_nprocs_conf(); 
    
    void* handle = dlopen("./libCodec.so", RTLD_LAZY | RTLD_GLOBAL);
    if(!handle){
        fprintf(stderr, "dlerror: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    
    encrypt_func = (void (*)(char *s, int key))dlsym(handle,"encrypt");
    decrypt_func = (void (*)(char *s, int key))dlsym(handle,"decrypt");

	key = atoi(argv[1]);
    int id = 0;
	char c;
	int counter = 0;
	int dest_size = 1024;
	char data[dest_size]; 
    strcpy(type, argv[2]);
    count_tasks = 0;
    memset(data, '\0', 1024);
	
    while ((c = getchar()) != EOF){
       data[counter] = c;
       counter++;

        if (counter == 1024){
            tasks.push_back(data);
    	    counter = 0;
        }
    }

    if (counter > 0){
        char lastData[counter];
        lastData[0] = '\0';
        strncat(lastData, data, counter);
        tasks.push_back(lastData);
    }
    for (int i = 0; i < num_of_threads; ++i)
    {
        pthread_t thread_id;

        pthread_create(&thread_id, NULL, &worker_thread, NULL);
        threads.push_back(thread_id);
    }
    for (int i = 0; i < threads.size(); i++)
    {
        pthread_join(threads.at(i), NULL);
    }
    for (size_t i = 0; i < count_tasks; i++)
    {
        cout<<all_data.at(i).c_str();
    }
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "usage_tracker/inc/tracker.h"



/*function get cpu info
  return -> numbers of cores */
long get_ncpu(void);



int main(int arg, char *args[]){
    fprintf(stdout,"TEST 1\n");
    void *cpu_inst;
    cpu_inst = init_tracker();
    pthread_t thread_read, thread_analyzer;
    fprintf(stdout,"TEST 2\n");
    pthread_create(&thread_read, NULL, thread_reader_func, cpu_inst);
    pthread_create(&thread_analyzer, NULL, thread_analyzer_func, cpu_inst);
    
    void** result;
    pthread_join(thread_read, result);
    pthread_join(thread_analyzer, result);
    return 0;
}

long  get_ncpu(void){
    long  n_processor =  sysconf(_SC_NPROCESSORS_ONLN);
    if(n_processor == -1){
        return -1;
    }
    return n_processor;
}


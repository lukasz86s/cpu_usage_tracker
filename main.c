#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "usage_tracker/inc/tracker.h"


pthread_t* threads_array;
Cpu cpu_inst;

/*function get cpu info
  return -> numbers of cores */
static long get_ncpu(void);
//function handles SIGTERM
static void sig_term(int sig);
// 
static void wdt_call(void);

int main(void){

    threads_array = create_threads();
    // SIGTERM test
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sig_term;
    sigaction(SIGTERM, &action, NULL);
    //-------------------------------//

    cpu_inst = init_tracker(get_ncpu(), wdt_call);
    
    start_threads(threads_array, cpu_inst);
    join_threads(threads_array);
    
    clean_cpu(cpu_inst);
    frre_threads_mem(threads_array);
    return 0;
}

//function handles SIGTERM
static void sig_term(int sig){
    //
    kill_thread(threads_array);
    clean_cpu(cpu_inst);
    frre_threads_mem(threads_array);
    printf("sig param %d\n", sig);
    exit(1);
  
}


static long  get_ncpu(void){
    long  n_processor =  sysconf(_SC_NPROCESSORS_ONLN);
    if(n_processor == -1){
        return -1;
    }
    return n_processor;
}

static void wdt_call(void){
    kill_thread(threads_array);
    exit(1);
    
}

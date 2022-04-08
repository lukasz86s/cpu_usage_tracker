#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "usage_tracker/inc/tracker.h"


pthread_t thread_read, thread_analyzer, thread_printer, thread_watchdog;
void** result;

/*function get cpu info
  return -> numbers of cores */
long get_ncpu(void);
//function handles SIGTERM
void sig_term(int sig){
    //

}

int main(int arg, char *args[]){
    // SIGTERM test
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sig_term;
    sigaction(SIGTERM, &action, NULL);
 
    fprintf(stdout,"TEST 1\n");
    void *cpu_inst;
    cpu_inst = init_tracker(get_ncpu());
    
    
    
    pthread_create(&thread_read, NULL, thread_reader_func, cpu_inst);
    pthread_create(&thread_analyzer, NULL, thread_analyzer_func, cpu_inst);
    pthread_create(&thread_analyzer, NULL, thread_printer_func, cpu_inst);
    pthread_create(&thread_watchdog, NULL, thread_watchdog_func, cpu_inst);
    
    
    pthread_join(thread_read, result);
    pthread_join(thread_analyzer, result);
    pthread_join(thread_printer, result);
    pthread_join(thread_watchdog, result);
    return 0;
}

long  get_ncpu(void){
    long  n_processor =  sysconf(_SC_NPROCESSORS_ONLN);
    if(n_processor == -1){
        return -1;
    }
    return n_processor;
}


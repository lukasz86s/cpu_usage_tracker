#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "usage_tracker/inc/tracker.h"



/*function get cpu info
  return -> numbers of cores */
long get_ncpu(void);



int main(int arg, char *args[]){

    printf("TEST\n");
    thread_reader_func(NULL);
    thread_analyzer_func(NULL);
    
    return 0;
}

long  get_ncpu(void){
    long  n_processor =  sysconf(_SC_NPROCESSORS_ONLN);
    if(n_processor == -1){
        return -1;
    }
    return n_processor;
}


#include <stdio.h>
/*function processes the string from lscpu cmd
  return -> numbers of cores */
int get_ncpu(void);

/*function read from /proc/stat*/
void* thread_reader(void *arg);

void* thread_analyzer(void *arg);

void* thread_printer(void *arg);

void* thread_watchdog(void *arg);

void* thread_logger(void *arg);


int main(int arg, char *args[]){
    printf("TEST");

}
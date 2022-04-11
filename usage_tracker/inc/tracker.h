#ifndef TRACKER_H
#define TRACKER_H
#include <pthread.h>
#define FILE_BUFFER_SIZE 100000
#define MAX_THREADS 5

typedef void *Cpu;
/* function initializes structures and variables
    n_cores: number of cpu cores
    wdt_call: function to call when watchdog triggers
    return => cpu instance
*/
void* init_tracker(long n_cores, void (* wdt_call)(void));
/*
retur array of created threads
*/
pthread_t* create_threads(void);
/*
free alocated memory
    arr: array of threads
*/ 
void frre_threads_mem(pthread_t* arr);
/*
starts threads
    arr: array of threads
    arg: argumet pas to thread function
    */
void start_threads(pthread_t* arr, Cpu arg);
/*
join all threads
    arr: array of threads
*/
void join_threads(pthread_t* arr);
/*
stops threads
    arr: array of threads
*/
void kill_thread(pthread_t* arr);

/*function read from /proc/stat*/
void* thread_reader_func(void* arg);

void* thread_analyzer_func(void* arg);

void* thread_printer_func(void* arg);

void* thread_watchdog_func(void* arg);

void* thread_logger_func(void* arg);

void* end_program(void* arg);

void clean_cpu(void* cpu);

#endif

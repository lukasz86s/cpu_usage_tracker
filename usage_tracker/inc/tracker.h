#ifndef TRACKER_H
#define TRACKER_H
#define FILE_BUFFER_SIZE 100000


/* function initializes structures and variables
    n_cores => numbers of cores
*/
void* init_tracker(int n_cores);

/*function read from /proc/stat*/
void* thread_reader_func(void *arg);

void* thread_analyzer_func(void *arg);

void* thread_printer_func(void *arg);

void* thread_watchdog_func(void *arg);

void* thread_logger_func(void *arg);



#endif
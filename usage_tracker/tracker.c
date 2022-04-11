#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "inc/tracker.h"
#include "inc/queue.h"
#include "inc/wdt.h"

//enums corelate to columns names in raw_data
enum {USER, NICE, SYSTEM, IDLE, IOWAIT, IRQ, SOFTIRQ, STEAL, GUEST, GUEST_NICE,};

typedef 
        struct _CpuInfo{
            int work;
            unsigned long nr_cores;
            int parameters_len;
            double* procent_use;
            char** labels;
            uint64_t** raw_data;
            uint64_t** prev_raw_data;
            Wdt* wdt;
            Queue* queue;
        }CpuInfo;

static CpuInfo* create_cpu(unsigned long n_cpu, int parameters_len);
//static void destory_cpu(CpuInfo* cpu);

/*
function calculates percent usage of cores
    prev_cpu_data: previous readed data
    cpu_data: currently readed data
*/ 
static double calculate_usage(uint64_t* prev_cpu_data, uint64_t* cpu_data){
    uint64_t prev_idle, prev_non_idle,\
     idle, non_idle,\
     prev_total, total; 

    //TODO: add sum macro
    prev_idle = prev_cpu_data[IDLE] + prev_cpu_data[IOWAIT];
    idle = cpu_data[IDLE] + cpu_data[IOWAIT];
    
    prev_non_idle = prev_cpu_data[USER] + prev_cpu_data[NICE] + prev_cpu_data[SYSTEM]\
     + prev_cpu_data[IRQ] + prev_cpu_data[SOFTIRQ] + prev_cpu_data[STEAL];
    
    non_idle = cpu_data[USER] + cpu_data[NICE] + cpu_data[SYSTEM]\
     + cpu_data[IRQ] + cpu_data[SOFTIRQ] + cpu_data[STEAL];
    
    prev_total = prev_idle + prev_non_idle;
    total = idle + non_idle;

    total =  total - prev_total;
    idle = idle - prev_idle;
    
    return (double)(total - idle) / (double)total;
}
/*
function initializes structures and variables
    n_cores: number of cpu cores
    wdt_call: function to call when watchdog triggers
    return => cpu instance
*/
void* init_tracker(long n_cores,  void (* wdt_call)(void)){

    CpuInfo* temp_cpu;
    temp_cpu = create_cpu((unsigned )n_cores , 10);
    // set wdt timers
    temp_cpu->wdt->wdt_call = wdt_call;
    set_wdt_all_timers(temp_cpu->wdt, 2ul);
    return (void*) temp_cpu;
}

/*
retur array of created threads
*/
pthread_t* create_threads(void){
    pthread_t* threads = (pthread_t*)calloc(MAX_THREADS, sizeof(pthread_t));
    return threads;
}
/*
free alocated memory
    arr: array of threads
*/ 
void frre_threads_mem(pthread_t* arr){
    free(arr);
}
/*
starts threads
    arr: array of threads
    */
void start_threads(pthread_t* arr, Cpu arg){

    pthread_create(&arr[0], NULL, thread_reader_func, arg);
    pthread_create(&arr[1], NULL, thread_analyzer_func, arg);
    pthread_create(&arr[2], NULL, thread_printer_func, arg);
    pthread_create(&arr[3], NULL, thread_watchdog_func, arg);
    pthread_create(&arr[4], NULL, end_program, arg);
    
}
void join_threads(pthread_t* arr){
    void** result = NULL;
    for(int i = 0; i < MAX_THREADS; i++){
        pthread_join(arr[i], result);
    }
    
}
/*
stops threads
    arr: array of threads
*/
void kill_thread(pthread_t* arr){
    for(int i = 0 ; i < MAX_THREADS; i++){
        pthread_cancel(arr[i]);
    }
}

/*function create label for finding cores in string
*/
static void copy_labels(CpuInfo* cpu){
       
        strcpy(cpu->labels[0], "cpu");
        for(unsigned long i = 1; i <= cpu->nr_cores; i++){
            sprintf(cpu->labels[i], "cpu%lu", (i-1));
        }
        

}


/*function create sturcture that describe cpu
    c_cpu => numbers of cores
    parameters_len => numbers of parameters to read from data
*/
static CpuInfo* create_cpu(unsigned long n_cpu, int parameters_len){

    // allocate memory for struct
    CpuInfo* temp_cpu = (CpuInfo* )malloc(sizeof(CpuInfo));
    // initialize with zeros
    memset(temp_cpu, 0, sizeof(CpuInfo) );
    //create queue
    temp_cpu->queue = create_queue();
    // set working state to 1
    temp_cpu->work = 1;
    // number of cores
    temp_cpu->nr_cores = n_cpu;
    //numbers of parameters to read
    temp_cpu->parameters_len = parameters_len;
    // procent usage all cores + cpu
    temp_cpu->procent_use = (double*) calloc((n_cpu + 1), sizeof(double) );
    // label + 1 for cores + cpu
    temp_cpu->labels = (char **) calloc((n_cpu + 1), sizeof(char*));
    // size of labels
    for(unsigned long i = 0; i < (n_cpu+1); i++){
        // max str 'cpuxx' (cores < 99) 
        temp_cpu->labels[i] = (char *) calloc(6, sizeof(char)); 
    }
    // (cores+1) * number of parameters
    temp_cpu->raw_data = (uint64_t**) calloc((n_cpu + 1), sizeof(uint64_t *));
    // the same for prev_raw_data
    temp_cpu->prev_raw_data = (uint64_t**) calloc((n_cpu + 1), sizeof(uint64_t *));
    //TODO: change to calloc, or memset to inint prev_raw_data values to 0
    for(unsigned long i = 0; i < (n_cpu+1); i++){
        temp_cpu->raw_data[i] = (uint64_t*) calloc((unsigned long)parameters_len, sizeof(uint64_t) );
        temp_cpu->prev_raw_data[i] = (uint64_t*) calloc((unsigned long)parameters_len, sizeof(uint64_t));
    }
    temp_cpu->wdt = create_wdt(3u); 
    // add labels
    copy_labels(temp_cpu);
    return temp_cpu;
}
static void destory_cpu(CpuInfo* cpu){
    //free queue memory
    destroy_queue(cpu->queue);
    //free watchdog memory 
    destroy_wdt(cpu->wdt);
    //
    free(cpu->procent_use);
    //data_raw rows are equal to numbers of labels
    for(unsigned long i = 0; i <= cpu->nr_cores; i++){
        free(cpu->raw_data[i]);
        free(cpu->prev_raw_data[i]);
        free(cpu->labels[i]);
    }
    free(cpu->labels);
    free(cpu->raw_data);
    free(cpu->prev_raw_data);
    free(cpu);

}
// function use to clean and end cpu instance
void clean_cpu(void* cpu){
    CpuInfo* cpu_ptr = (CpuInfo*)cpu;
    destory_cpu(cpu_ptr);
}

/*function copy raw data from file
    buff => pointer to bufor
    */
static int copy_file( char* buff){
    FILE* file;
    size_t n_char;

    file = fopen("/proc/stat", "rb");
    if(file== NULL){
        perror("ERROR: pointer to the file is NULL");
        return 1;
    }
    n_char = fread(buff, 1, FILE_BUFFER_SIZE, file);
    fclose(file);

    if(n_char == 0){
        perror("ERROR: 0 char readed from file");
        return 2;
    }
    return 0;
}

/* 

*/
void* thread_reader_func(void *arg){
    //TODO: replace FILE_BUFER_SIZE by couting data size func
    CpuInfo* cpu_info = (CpuInfo*) arg;
    char cpu_raw_data[FILE_BUFFER_SIZE];
    int file_copy_effect = 0;

    //initialize memory
    memset(cpu_raw_data, 0, FILE_BUFFER_SIZE );

    while(cpu_info->work){
        sleep(1);
        //reset watchdog timer
        reset_wdt_timer(cpu_info->wdt, 0ul);
        //copy data from file
        file_copy_effect = copy_file(cpu_raw_data);
        
        //don `t copy file if is epty or have read errors
        switch(file_copy_effect)
        {
        case 1:
            return((void *) 1);
        case 2:
            continue;
        default:
            // add data pointer to queue
            append_queu(cpu_info->queue, cpu_raw_data, FILE_BUFFER_SIZE);
            break;
        }
    }
    return NULL;
}

/* 

*/
void* thread_analyzer_func(void *arg){
    CpuInfo* cpu_info = (CpuInfo*) arg;
    //CpuInfo *cpu_info = (CpuInfo* ) arg;
    char cpu_raw_data[FILE_BUFFER_SIZE];
    char* token = NULL;
    unsigned long cpu_index = 0;
    //initialize memory
    memset(cpu_raw_data, 0, FILE_BUFFER_SIZE );

    while (cpu_info->work){
        sleep(1);
        //reset watchdog timer
        reset_wdt_timer(cpu_info->wdt, 1ul);
        //TODO: move block to function
        
        //decrease_queue(queue1, cpu_raw_data, FILE_BUFFER_SIZE) ;
        if(decrease_queue(cpu_info->queue, cpu_raw_data, FILE_BUFFER_SIZE) != 0){
            perror( "nothing to read\n");
           continue;
        }
        //reset cpu_index;
        cpu_index = 0;
        //tokenize data
        token = strtok(cpu_raw_data, " ");
        
        while(token != NULL){
            //searching for the beginning of each core data
            if(strcmp(cpu_info->labels[cpu_index], token) == 0){ 
                //convert nex ten tokens to int and add data to cpu_struct
                for(int i = 0 ; i < cpu_info->parameters_len; i++){
                    //copy raw_data to prev_raw_data
                    // needded for calculate usage percent
                    cpu_info->prev_raw_data[cpu_index][i] = cpu_info->raw_data[cpu_index][i];
                    token = strtok(NULL, " \n");
                    //convert to int and save values in raw_data
                    cpu_info->raw_data[cpu_index][i] = (uint64_t)atoi(token);
                }
                //end of cores , breake
                if(cpu_index == cpu_info->nr_cores){
                    //end of data needed
                    break;
                }
                cpu_index++;
            }
            token = strtok(NULL, " \n");
        }
        for(unsigned long i = 0 ; i <= cpu_info->nr_cores; i++){
            cpu_info->procent_use[i] = calculate_usage(cpu_info->prev_raw_data[i], cpu_info->raw_data[i]);
        }
    }
    return NULL;
}
static void get_bar_width(int procent_usage, char* bar){

        for(int i = 0; i < 50; i++){
            if(i <= procent_usage)
                bar[i] = '|';
            else
                bar[i] = ' '; 
        }
}
void* thread_printer_func(void *arg){
    CpuInfo* cpu_info = (CpuInfo*) arg;
    char usage_bar_str[51];
    char cpu_str[7];
    // initialize arrays
    memset(usage_bar_str, 0, 51);
    memset(cpu_str, 0, 7);
    usage_bar_str[50] = '\0';
    //clear screen
    system("clear");
    while (cpu_info->work)
    {   
        //reset watchdog timer
        reset_wdt_timer(cpu_info->wdt, 2ul);

        for(unsigned long  i = 0; i <= cpu_info->nr_cores; i++){
            if(i == 0 ){
                strcpy(cpu_str, "CPU  ");
            }else{
                sprintf(cpu_str, "Core%lu", (i-1));
            }
            get_bar_width((int)(cpu_info->procent_use[i] * 50.0), usage_bar_str);
            printf("%s[%s%.2f%%]\n",cpu_str, usage_bar_str, (cpu_info->procent_use[i] * 100.0));
        }
        printf("press enter to exit\n");
        sleep(1);
        system("clear");
    }
    
    return NULL;
}
void* end_program(void* arg){
    CpuInfo* cpu = (CpuInfo*) arg;
    getchar();//wait for any key
    cpu->work = 0; // stops lups in threads
    return NULL;
}

void* thread_watchdog_func(void *arg){
    CpuInfo* cpu_info = (CpuInfo*) arg;
    while(cpu_info->work){
    // decrease and check timers.calls selected function if wdt trigger 
    check_wdt(cpu_info->wdt);
    sleep(1);
    }
    return NULL;
}


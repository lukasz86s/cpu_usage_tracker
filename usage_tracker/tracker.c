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
            Queue* queue;
            int nr_cores;
            int parameters_len;
            double* procent_use;
            char** labels;
            uint64_t** raw_data;
            uint64_t** prev_raw_data;
            Wdt* wdt;
        }CpuInfo;

static CpuInfo* create_cpu(int n_cpu, int parameters_len);
//static void destory_cpu(CpuInfo* cpu);

/*function calculates percent usage of cores
    prev_cpu_data => previous readed data
    cpu_data => currently readed data
*/ 
static double calculate_usage(uint64_t* prev_cpu_data, uint64_t* cpu_data){
    uint64_t prev_idle, prev_non_idle,\
     idle, non_idle,\
     prev_total, total; 

  
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

void* init_tracker(int n_cores){

    CpuInfo* temp_cpu;
    temp_cpu = (CpuInfo*) malloc(sizeof(CpuInfo));
    temp_cpu = create_cpu(2 , 10);
    return (void*) temp_cpu;
}

/*function create label for finding cores in string
*/
static void copy_labels(CpuInfo* cpu){
       
        strcpy(cpu->labels[0], "cpu");
        for(int i = 1; i <= cpu->nr_cores; i++){
            sprintf(cpu->labels[i], "cpu%d", (i-1));
        }
        

}
//TODO: implemet functio triggered on wathdog
// maby move to wdt file 
// try trigger SIGREEG in function?
void wdt_call(void){
    printf(" call wdt func \n");
}
/*function create sturcture that describe cpu
    c_cpu => numbers of cores
    parameters_len => numbers of parameters to read from data
*/
static CpuInfo* create_cpu(int n_cpu, int parameters_len){

    // allocate memory for struct
    CpuInfo* temp_cpu = (CpuInfo* )malloc(sizeof(CpuInfo));
    //create queue
    temp_cpu->queue = create_queue();
    // number of cores
    temp_cpu->nr_cores = n_cpu;
    //numbers of parameters to read
    temp_cpu->parameters_len = parameters_len;
    // procent usage all cores + cpu
    temp_cpu->procent_use = (double*) malloc(sizeof(double) * (n_cpu + 1));
    // label + 1 for cores + cpu
    temp_cpu->labels = (char **) malloc(sizeof(char*) * (n_cpu + 1));
    // size of labels
    for(int i = 0; i < (n_cpu+1); i++){
        // max str 'cpuxx' (cores < 99) 
        temp_cpu->labels[i] = (char *) malloc(sizeof(char) * 6); 
    }
    // (cores+1) * number of parameters
    temp_cpu->raw_data = (uint64_t**) malloc(sizeof(uint64_t *) * (n_cpu + 1));
    // the same for prev_raw_data
    temp_cpu->prev_raw_data = (uint64_t**) malloc(sizeof(uint64_t *) * (n_cpu + 1));
    //TODO: add memset to inint prev_raw_data values to 0
    for(int i = 0; i < (n_cpu+1); i++){
        temp_cpu->raw_data[i] = (uint64_t*) malloc(sizeof(uint64_t*) * parameters_len);
        temp_cpu->prev_raw_data[i] = (uint64_t*) malloc(sizeof(uint64_t*) * parameters_len);
    }
    temp_cpu->wdt = create_wdt(3, wdt_call); 
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
    for(int i = 0; i <= cpu->nr_cores; i++){
        free(cpu->raw_data[i]);
        free(cpu->prev_raw_data[i]);
        free(cpu->labels[i]);
    }
    free(cpu->labels);
    free(cpu->raw_data);
    free(cpu->prev_raw_data);
    free(cpu);

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
    CpuInfo* cpu_info1 = (CpuInfo*) arg;
    char cpu_raw_data[FILE_BUFFER_SIZE];
    int file_copy_effect;
    while(1){
        sleep(1);
        //copy data from file
        file_copy_effect = copy_file(cpu_raw_data);
        
        //don `t copy file if is epty or have read errors
        switch(file_copy_effect)
        {
        case 1:
            return((void *) 1);
            break;
        case 2:
            continue;
            break;
        default:
            // add data pointer to queue
            append_queu(cpu_info1->queue, cpu_raw_data, FILE_BUFFER_SIZE);
            break;
        }
    }
    return NULL;
}

/* 

*/
void* thread_analyzer_func(void *arg){
    CpuInfo* cpu_info1 = (CpuInfo*) arg;
    //CpuInfo *cpu_info = (CpuInfo* ) arg;
    char cpu_raw_data[FILE_BUFFER_SIZE];
    char* token;
    int cpu_index;

    while (1){
        sleep(1);
        //TODO: move block to function
        
        //decrease_queue(queue1, cpu_raw_data, FILE_BUFFER_SIZE) ;
        if(decrease_queue(cpu_info1->queue, cpu_raw_data, FILE_BUFFER_SIZE) != 0){
            perror( "nothing to read\n");
           continue;
        }
        //reset cpu_index;
        cpu_index = 0;
        //tokenize data
        token = strtok(cpu_raw_data, " ");
        
        while(token != NULL){
            //searching for the beginning of each core data
            if(strcmp(cpu_info1->labels[cpu_index], token) == 0){ 
                //convert nex ten tokens to int and add data to cpu_struct
                for(int i = 0 ; i < cpu_info1->parameters_len; i++){
                    //copy raw_data to prev_raw_data
                    // needded for calculate usage percent
                    cpu_info1->prev_raw_data[cpu_index][i] = cpu_info1->raw_data[cpu_index][i];
                    token = strtok(NULL, " \n");
                    //convert to int and save values in raw_data
                    cpu_info1->raw_data[cpu_index][i] = atoi(token);
                }
                //end of cores , breake
                if(cpu_index == cpu_info1->nr_cores){
                    //end of data needed
                    break;
                }
                cpu_index++;
            }
            token = strtok(NULL, " \n");
        }
        for(int i = 0 ; i <= cpu_info1->nr_cores; i++){
            cpu_info1->procent_use[i] = calculate_usage(cpu_info1->prev_raw_data[i], cpu_info1->raw_data[i]);
            //fprintf(stdout, "core : %lf \n",cpu_info1->procent_use[i]);
        }
    }
    return NULL;
}
void get_bar_width(int procent_usage, char* bar){

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

    usage_bar_str[50] = '\0';

    while (1)
    {   
        for(int i = 0; i <= cpu_info->nr_cores; i++){
            if(i == 0 ){
                strcpy(cpu_str, "CPU  ");
            }else{
                sprintf(cpu_str, "Core%d", (i-1));
            }
            get_bar_width((int)(cpu_info->procent_use[i] * 50.0), usage_bar_str);
            printf("%s[%s%.2f%%]\n",cpu_str, usage_bar_str, (cpu_info->procent_use[i] * 100.0));
        }
        sleep(1);
        system("clear");
    }
    
    return NULL;
}

void* thread_watchdog_func(void *arg){
    CpuInfo* cpu_info = (CpuInfo*) arg;
    while(1){
    // decrease and check timers.calls selected function if wdt trigger 
    check_wdt(cpu_info->wdt);
    sleep(1);
    }
    return NULL;
}
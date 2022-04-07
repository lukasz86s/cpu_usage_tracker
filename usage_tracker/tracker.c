#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "inc/tracker.h"
#include "inc/queue.h"


typedef 
        struct _CpuInfo{
            Queue* queue;
            int nr_cores;
            int parameters_len;
            int* procent_use;
            char** labels;
            int** raw_data;
        }CpuInfo;

static CpuInfo* create_cpu(int n_cpu, int parameters_len);
static void destory_cpu(CpuInfo* cpu);

void* init_tracker(void){

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
/*function create sturcture that describe cpu
    c_cpu => numbers of cores
    parameters_len => numbers of parameters to read from data*/

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
    temp_cpu->procent_use = (int*) malloc(sizeof(int) * (n_cpu + 1));
    // label + 1 for cores + cpu
    temp_cpu->labels = (char **) malloc(sizeof(char*) * (n_cpu + 1));
    // size of labels
    for(int i = 0; i < (n_cpu+1); i++){
        // max str 'cpuxx' (cores < 99) 
        temp_cpu->labels[i] = (char *) malloc(sizeof(char) * 6); 
    }
    // (cores+1) * number of parameters
    temp_cpu->raw_data = (int **) malloc(sizeof(int *) * (n_cpu + 1));
    for(int i = 0; i < (n_cpu+1); i++){
        temp_cpu->raw_data[i] = (int*) malloc(sizeof(int*) * parameters_len);
    }  
    // add labels
    copy_labels(temp_cpu);
    return temp_cpu;
}
static void destory_cpu(CpuInfo* cpu){

    //free queue memory
    destroy_queue(cpu->queue);
    //
    free(cpu->procent_use);
    //data_raw rows are equal to numbers of labels
    for(int i = 0; i <= cpu->nr_cores; i++){
        free(cpu->raw_data[i]);
        free(cpu->labels[i]);
    }
    free(cpu->labels);
    free(cpu->raw_data);
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
        sleep(1.5);
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
            fprintf(stdout, "nothing to read\n");
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
                fprintf(stdout, "\n\n\n%s\n", token);
                for(int i = 0 ; i < cpu_info1->parameters_len; i++){

                    token = strtok(NULL, " \n");
                    //convert to int and save values in raw_data
                    cpu_info1->raw_data[cpu_index][i] = atoi(token);
                }
                fprintf(stdout, "OUT :%s\n", token);
                if(cpu_index == cpu_info1->nr_cores){
                    //end of data needed
                    break;
                }
                cpu_index++;
            }
            token = strtok(NULL, " \n");
        }
        
    }
    return NULL;
}
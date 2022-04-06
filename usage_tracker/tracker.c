#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "inc/tracker.h"
#include "inc/queue.h"

typedef 
        struct _CpuInfo{
            int nr_cores;
            int* procent_use;
            int* raw_data[10];
        }CpuInfo;


int n_cpu = 2;
Queue* queue1 = NULL;

int copy_file( char* buff);

void* thread_reader_func(void *arg){
    //TODO: replace FILE_BUFER_SIZE by couting data size func
    char cpu_raw_data[FILE_BUFFER_SIZE];
    int file_copy_effect;

    // create a queue if it hasn`t already been done
    if(queue1 == NULL){
        queue1 = create_queue();
    }
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
            append_queu(queue1, cpu_raw_data, FILE_BUFFER_SIZE);
            break;
        }
    }
    return NULL;
}

/*function copy raw data from file
    path => path to file
    buff => pointer to bufor
    */
int copy_file( char* buff){
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


void* thread_analyzer_func(void *arg){

    char cpu_raw_data[FILE_BUFFER_SIZE];
    char* token;
    char cpu_labels[n_cpu+1][6];
    int cpu_index = 0;
    

    //creating labels for comparasion
    strcpy(cpu_labels[0], "cpu");
    for(int i = 1; i <= n_cpu; i++){
        sprintf(cpu_labels[i], "cpu%d", (i-1));
    }
    while (1){
        sleep(1);
        //TODO: move block to function
        
        //decrease_queue(queue1, cpu_raw_data, FILE_BUFFER_SIZE) ;
        if(decrease_queue(queue1, cpu_raw_data, FILE_BUFFER_SIZE) != 0){
            fprintf(stdout, "nothing to read\n");
           continue;
        }
        //reset cpu_index;
        cpu_index = 0;
        //tokenize data
        token = strtok(cpu_raw_data, " ");
        
        while(token != NULL){
            //searching for the beginning of each core data
            if(strcmp(cpu_labels[cpu_index], token) == 0){ 
                //TODO: convert nex ten tokens to int
                fprintf(stdout, "OUT :%s\n", token);
                if(cpu_index == (n_cpu + 1)){
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
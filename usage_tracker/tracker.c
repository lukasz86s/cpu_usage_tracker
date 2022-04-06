#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tracker.h"
#include "inc/queue.h"


int n_cpu = 2;
Queue* queue1 = NULL;


void* thread_reader_func(void *arg){
    FILE *cpu_info;
    size_t n_char;
    char* cpu_raw_data;

    // create a queue if it hasn`t already been done
    if(queue1 == NULL){
        queue1 = create_queue();
    }
    //alocate memory for data
    cpu_raw_data = (char* ) malloc(sizeof(char) * FILE_BUFFER_SIZE);

    cpu_info = fopen("/proc/stat", "rb");
    if(cpu_info == NULL){
        perror("ERROR: pointer to the file is NULL");
        return NULL;
    }
    n_char = fread(cpu_raw_data, 1, FILE_BUFFER_SIZE, cpu_info);
    fclose(cpu_info);

    append_queu(queue1, cpu_raw_data);

    return NULL;
}

void* thread_analyzer_func(void *arg){

    char* cpu_raw_data;
    char *token;
    char cpu_labels[n_cpu+1][6];
    int cpu_index = 0;
    

    //creating labels for comparasion
    strcpy(cpu_labels[0], "cpu");
    for(int i = 1; i <= n_cpu; i++){
        sprintf(cpu_labels[i], "cpu%d", (i-1));
    }
    //TODO: move block to function
    cpu_raw_data = decrease_queue(queue1);
    {
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
    free(cpu_raw_data);
    return NULL;
}
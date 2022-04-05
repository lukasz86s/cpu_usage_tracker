#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILE_BUFFER_SIZE 100000
#define MAX_LINE 256
/*function processes the string from lscpu cmd
  return -> numbers of cores */
long get_ncpu(void);

/*function read from /proc/stat*/
void* thread_reader_func(void *arg);

void* thread_analyzer_func(void *arg);

void* thread_printer_func(void *arg);

void* thread_watchdog_func(void *arg);

void* thread_logger_func(void *arg);


char cpu_info_buffer[FILE_BUFFER_SIZE];
long n_cpu = 0;
int main(int arg, char *args[]){

    printf("TEST\n");
    n_cpu = get_ncpu();
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

void* thread_reader_func(void *arg){
    
    FILE *cpu_info;
    size_t n_char;
    cpu_info = fopen("/proc/stat", "rb");
    if(cpu_info == NULL){
        perror("ERROR: pointer to the file is NULL");
        return NULL;
    }
    n_char = fread(cpu_info_buffer, 1, FILE_BUFFER_SIZE, cpu_info);
    fclose(cpu_info);
    return NULL;
}

void* thread_analyzer_func(void *arg){
    char *token;
    char cpu_labels[n_cpu+1][6];
    int cpu_index = 0;

    //creating labels for comparasion
    strcpy(cpu_labels[0], "cpu");
    for(int i = 1; i <= n_cpu; i++){
        sprintf(cpu_labels[i], "cpu%d", (i-1));
    }
    
    {
        //tokenize data
        token = strtok(cpu_info_buffer, " ");
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
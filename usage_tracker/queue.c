
#include "./inc/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*function check if the queue is empty  */
static int is_empty(Queue* queue);
/*function check if the queue is full  */
static int is_full(Queue* queue);

Queue* create_queue(void){
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->head = queue->size = 0;
    queue->tail = QUEUE_LEN - 1;
    return queue;
}

void destroy_queue(Queue* queue){
    for(int i = 0; i < QUEUE_LEN; i++){
        // if program was intrupted, check if something steyed in array
        if(queue->array[i] != NULL){
            free(queue->array[i]);

        }
    }
    // destroy queue struct
    free(queue);    
    
}
/*function check if the queue is empty  */
static int is_empty(Queue* queue){
    return (queue->size == 0);
}
/*function check if the queue is full  */
static int is_full(Queue* queue){
    return (queue->size == QUEUE_LEN);
}

/*functio add item to queue*/
void append_queu(Queue* queue, char* item, int len){
    
    if(is_full(queue)){
        perror("ERROR: queue is full");
        return;
    }
    char* temp_data = (char* ) calloc((unsigned long)len, sizeof(char));
    //copy data 
    strncpy(temp_data, item,(unsigned long) len);
    // checks if is out of array
    queue->tail = (queue->tail +1) % QUEUE_LEN;
    queue->array[queue->tail] = temp_data;
    queue->size += 1;
}

/*function remove an item from queue*/
int decrease_queue(Queue* queue, char* buff, int len){

    if(is_empty(queue)){
        perror("ERROR: queue is empty\n");
        return 1;
    }
    //copydata
    strncpy(buff, queue->array[queue->head], (unsigned long)len);
    // free pointer memory
    free(queue->array[queue->head]);
    //set pointer to null
    queue->array[queue->head] = NULL;
    //incres tail
    queue->head = (queue->head + 1) % QUEUE_LEN;
    queue->size -= 1;

    return 0;
}

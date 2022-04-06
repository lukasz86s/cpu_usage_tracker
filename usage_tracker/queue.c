
#include "./inc/queue.h"
#include <stdio.h>
#include <stdlib.h>


Queue* create_queue(void){
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->head = queue->size = 0;
    queue->tail = QUEUE_LEN - 1;
    return queue;
}

void destroy_queue(Queue* queue){
    // free array memory
    for(int i = 0; i < QUEUE_LEN; i++){
        if (queue->array[i] != NULL){
            free(queue->array[i]);
        }
    // destroy queue struct
    free(queue);    
    }
}
/*function check if the queue is empty  */
int is_empty(Queue* queue){
    return (queue->size == 0);
}
/*function check if the queue is full  */
int is_full(Queue* queue){
    return (queue->size == QUEUE_LEN);
}

/*functio add item to queue*/
void append_queu(Queue* queue, char* item){
    
    if(is_full(queue)){
        return;
    }
    // checks if is out of array
    queue->tail = (queue->tail +1) % QUEUE_LEN;
    queue->array[queue->tail] = item;
    queue->size += 1;
}

/*function remove an item from queue*/
char* decrease_queue(Queue* queue){

    if(is_empty(queue)){
        return NULL;
    }
    char* item = queue->array[queue->head];
    queue->head = (queue->head + 1) % QUEUE_LEN;
    queue->size -= 1;
    // remove pointer from list
    queue->array[(queue->head -1)] = NULL;
    return item;
}
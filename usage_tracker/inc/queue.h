#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_LEN 10

typedef
struct _Queue{
    int head, tail, size;
    char * array[QUEUE_LEN];
}Queue;

/*function create new queue*/
Queue* create_queue(void);

/*function free memory of Queue*/
void destroy_queue(Queue* queue);

/*functio add item to queue*/
void append_queu(Queue* queue, char* item, int len);

/*function remove an item from queue*/
int decrease_queue(Queue* queue, char* buff, int len);


#endif
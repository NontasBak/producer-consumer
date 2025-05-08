#ifndef PROD_CONS_H
#define PROD_CONS_H

#include <pthread.h>
#include <sys/time.h>

#define QUEUESIZE 10
#define LOOP 100000
#define NUM_ANGLES 10

struct workFunction {
    void* (*work)(void*);
    void* arg;
    struct timeval enqueue_time;
    struct timeval dequeue_time;
};

typedef struct {
    struct workFunction buf[QUEUESIZE];
    long head, tail;
    int full, empty;
    pthread_mutex_t* mut;
    pthread_cond_t *notFull, *notEmpty;
    double total_wait_time;
    int producers_done;
} queue;

queue* queueInit(void);
void queueDelete(queue* q);
void queueAdd(queue* q, struct workFunction in);
void queueDel(queue* q, struct workFunction* out);

void* producer(void* args);
void* consumer(void* args);
void* calculate_sine(void* arg);

#endif  // PROD_CONS_H

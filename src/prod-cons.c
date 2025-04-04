#include "prod-cons.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define PI 3.1415

int main(int argc, char *argv[]) {
    int p, q;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_producers> <num_consumers>\n", argv[0]);
        exit(1);
    }
    p = atoi(argv[1]);
    q = atoi(argv[2]);

    queue *fifo;
    pthread_t pro[p], con[q];

    fifo = queueInit();
    if (fifo == NULL) {
        fprintf(stderr, "main: Queue Init failed.\n");
        exit(1);
    }

    for (int i = 0; i < p; i++) {
        pthread_create(&pro[i], NULL, producer, fifo);
    }

    for (int i = 0; i < q; i++) {
        pthread_create(&con[i], NULL, consumer, fifo);
    }

    for (int i = 0; i < p; i++) {
        pthread_join(pro[i], NULL);
    }

    pthread_mutex_lock(fifo->mut);
    fifo->producers_done = 1;                // Producers have finished
    pthread_cond_broadcast(fifo->notEmpty);  // Needed cause consumers might be stuck
    pthread_mutex_unlock(fifo->mut);

    // Now the consumers can return when they finish
    for (int i = 0; i < q; i++) {
        pthread_join(con[i], NULL);
    }

    printf("----- WAITING TIME STATISTICS -----\n");
    printf("Total items processed: %d\n", LOOP * p);
    printf("Average waiting time: %.6f ms\n",
           fifo->total_wait_time / (LOOP * p));
    queueDelete(fifo);

    return 0;
}

void *producer(void *args) {
    queue *fifo = (queue *)args;
    srand(time(NULL));

    for (int i = 0; i < LOOP; i++) {
        // Create work item
        struct workFunction wf;

        double *angles = malloc(sizeof(double) * NUM_ANGLES);
        for (int j = 0; j < NUM_ANGLES; j++) {
            // Generate random angles between 0 and 2π
            angles[j] = ((double)rand() / RAND_MAX) * 2 * PI;
        }

        wf.work = calculate_sine;
        wf.arg = angles;

        pthread_mutex_lock(fifo->mut);
        while (fifo->full) {
            // printf("producer: queue FULL.\n");
            pthread_cond_wait(fifo->notFull, fifo->mut);
        }

        // Record time when item is put into queue
        gettimeofday(&wf.enqueue_time, NULL);
        queueAdd(fifo, wf);

        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notEmpty);
    }

    return (NULL);
}

void *consumer(void *args) {
    queue *fifo = (queue *)args;
    struct workFunction wf;
    struct timeval tv_diff;
    double wait_time_ms;

    while (1) {
        pthread_mutex_lock(fifo->mut);

        // Check if queue is empty AND all producers are done
        while (fifo->empty && !fifo->producers_done) {
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }

        // Exit if queue is empty and producers are done
        if (fifo->empty && fifo->producers_done) {
            pthread_mutex_unlock(fifo->mut);
            break;
        }

        queueDel(fifo, &wf);
        gettimeofday(&wf.dequeue_time, NULL);

        // Calculate waiting time
        timersub(&wf.dequeue_time, &wf.enqueue_time, &tv_diff);
        wait_time_ms = tv_diff.tv_sec * 1000.0 + tv_diff.tv_usec / 1000.0;

        fifo->total_wait_time += wait_time_ms;

        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);

        // Execute the function
        void *result = wf.work(wf.arg);

        free(wf.arg);
        free(result);
    }

    return (NULL);
}

void *calculate_sine(void *arg) {
    double *angles = (double *)arg;
    double *results = malloc(sizeof(double) * NUM_ANGLES);

    for (int i = 0; i < NUM_ANGLES; i++) {
        results[i] = sin(angles[i]);
    }

    return results;
}
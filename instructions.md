Modify the prod-cons.c example so that:

1. The FIFO queue holds items of type:
```c
struct workFunction {
  void * (*work)(void *);
  void * arg;
}
```

2. It operates with p producer threads that put pointers to functions with the above structure into the common queue, and q consumer threads that take the pointers from the queue and execute the function. The functions in the queue do something simple, e.g., calculate the sine for ten angles. The producer thread ensures that the arguments make sense and are ready for reading and writing.

3. Do not use sleep(), as artificial delay is not needed, nor is the second loop. Make the first loop repeat for a specific large number of iterations. Make the consumer loop "while(1)".

Prepare a report of up to 2 pages presenting statistics on the waiting time that elapses from the moment a producer puts something in the queue until a consumer retrieves it (but not executes it), on a computer with at least two cores. Use the gettimeofday() function to find the timestamp.

What is the number of consumer threads in your system that minimizes the average waiting time?

Use a sufficient number of experiments until your measurements stabilize.

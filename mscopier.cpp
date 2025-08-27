#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
// #include "common_threads.h"
#include "zem.h"

const int MAX = 1;
Zem empty(MAX);
Zem full(0);
Zem mutex(1);
int loops = 5;

int buffer[MAX];
int fill = 0;
int use = 0;

void put(int value)
{
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
}

int get()
{
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    return tmp;
}

void *consumer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        full.wait();     // Line C1
        mutex.wait();    // Line C1.5 (lock)
        int tmp = get(); // Line C2
        mutex.post();    // Line C2.5 (unlock)
        empty.post();    // Line C3
        printf("%d\n", tmp);
    }
    return nullptr;
}

void *producer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        empty.wait(); // Line P1
        mutex.wait(); // Line P1.5 (lock)
        put(i);       // Line P2
        mutex.post(); // Line P2.5 (unlock)
        full.post();  // Line P3
    }
    return nullptr;
}

int main(int argc, char *argv[])
{
    pthread_t producerThread;
    pthread_t consumerThread;
    pthread_create(&producerThread, nullptr, producer, nullptr);
    pthread_create(&consumerThread, nullptr, consumer, nullptr);

    pthread_join(producerThread, nullptr);
    pthread_join(consumerThread, nullptr);

    return 0;
}

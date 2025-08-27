#include <iostream>
#include <pthread.h>
#include "zem.h"

// FIX-ME: must check return type of all api functions
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
    // fix-me:  check if source file exists...
    if (argc != 4)
    {
        std::cerr << "Error: requires three arguements\n";
        return 1;
    }
    std::string numThreadsStr = argv[1];
    int threads = 0;

    try
    {
        threads = std::stoi(numThreadsStr);
        if (threads < 2 || threads > 10)
        {
            std::cerr << "Error: number of files must be between 2 & 10\n";
            return 1;
        }
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Error: first arguement must be a valid integer\n";
        return 1;
    }
    catch (const std::out_of_range &)
    {
        std::cerr << "Error: integer out of range\n";
        return 1;
    }

    char *sourcefile = argv[2]; // char arrays
    char *destFile = argv[3];   // char arrays

    std::string source = "source";
    std::string txt = ".txt";
    pthread_t producerThreads[threads];
    pthread_t consumerThreads[threads];

    for (int i = 0; i < threads; i++)
    {
        std::string name = source + std::to_string(i + 1) + txt;
        // *file is created on the heap so it lives no past the loop iteration
        // fileData *file = new fileData(name, sourceDir, destDir); // char arrays are implcitly changed to std::string
        pthread_create(&producerThreads[i], nullptr, producer, sourcefile);
        pthread_create(&consumerThreads[i], nullptr, consumer, destFile);
    }

    pthread_t producerThread;
    pthread_t consumerThread;
    pthread_create(&producerThread, nullptr, producer, nullptr);
    pthread_create(&consumerThread, nullptr, consumer, nullptr);

    pthread_join(producerThread, nullptr);
    pthread_join(consumerThread, nullptr);

    return 0;
}

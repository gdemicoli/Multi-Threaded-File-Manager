#include <iostream>
#include <pthread.h>
#include <fstream>
#include "zem.h"

const int MAX = 20;
Zem empty(MAX);
Zem full(0);
Zem queueMutex(1);

std::string buffer[MAX];
int fill = 0;
int use = 0;
// basic prod/cons methods for placing and retrieving data
void put(std::string line)
{
    buffer[fill] = line;
    fill = (fill + 1) % MAX;
}

std::string get()
{

    std::string tmp = buffer[use];
    use = (use + 1) % MAX;
    return tmp;
}

void *consumer(void *arg)
{
    std::ofstream *outfile = static_cast<std::ofstream *>(arg);

    // Consumer loop ensures mutual exclusion with use of binary semaphore
    // empty & full represent buffer semaphore
    while (true)
    {
        full.wait();
        queueMutex.wait();
        std::string line = get();

        if (line == "__STOP__")
        {
            queueMutex.post();
            empty.post();
            break;
        }

        *outfile << line << "\n";
        queueMutex.post();
        empty.post();
    }
    return nullptr;
}

void *producer(void *arg)
{
    std::ifstream *file = static_cast<std::ifstream *>(arg);

    if (!file || !file->is_open())
    {
        std::cerr << "File not open in thread!" << std::endl;
        return nullptr;
    }
    std::string line;

    // producer loop ensures mutual exclusion with use of binary semaphore
    // empty & full represent buffer semaphore
    while (true)
    {
        empty.wait();
        queueMutex.wait();
        if (!std::getline(*file, line))
        {
            put("__STOP__");
            queueMutex.post();
            full.post();
            break;
        }
        put(line);
        queueMutex.post();
        full.post();
    }

    return nullptr;
}

int main(int argc, char *argv[])
{

    std::string numThreadsStr = argv[1];
    int threads = 0;
    if (argc != 4)
    {
        std::cerr << "Error: requires three arguements\n";
        return 1;
    }
    char *endptr;
    threads = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || threads < 2 || threads > 10)
    {
        std::cerr << "Error: number of threads must be a valid integer between 2 & 10\n";
        return 1;
    }

    char *sourcefile = argv[2];
    char *destFile = argv[3];

    std::ifstream file(sourcefile);

    if (!file.is_open())
    {
        std::cerr << "File not found" << std::endl;
        return 1;
    }

    std::ofstream outfile(destFile, std::ios::out);
    if (!outfile.is_open())
    {
        std::cerr << "Could not open destination file!" << std::endl;
        return 1;
    }

    pthread_t producerThreads[threads];
    pthread_t consumerThreads[threads];
    int returnProducer;
    int returnConsumer;
    // Creates consumer and producer threads, calling the respective function and arguements
    for (int i = 0; i < threads; i++)
    {
        returnProducer = pthread_create(&producerThreads[i], nullptr, producer, &file);
        if (returnProducer != 0)
        {
            std::cerr << "Error when making producer thread" << std::endl;
            return 1;
        }

        returnConsumer = pthread_create(&consumerThreads[i], nullptr, consumer, &outfile);
        if (returnConsumer != 0)
        {
            std::cerr << "Error when making consumer thread" << std::endl;
            return 1;
        }
    }
    for (int i = 0; i < threads; i++)
    {
        returnProducer = pthread_join(producerThreads[i], nullptr);

        if (returnProducer != 0)
        {
            std::cerr << "Error when joining producer thread" << std::endl;
            return 1;
        }
    }
    file.close();

    for (int i = 0; i < threads; i++)
    {
        returnConsumer = pthread_join(consumerThreads[i], nullptr);
        if (returnConsumer != 0)
        {
            std::cerr << "Error when joining consumer thread" << std::endl;
            return 1;
        }
    }
    outfile.close();

    return 0;
}

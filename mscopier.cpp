#include <iostream>
#include <pthread.h>
#include <fstream>
#include "zem.h"

// FIX-ME: must check return type of all api functions
const int MAX = 15;
Zem empty(MAX);
Zem full(0);
Zem queueMutex(1);
Zem fileLock(1);
int loops = 5;

std::string buffer[MAX];
int fill = 0;
int use = 0;

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
    int i;
    for (i = 0; i < loops; i++)
    {
        full.wait();             // Line C1
        queueMutex.wait();       // Line C1.5 (lock)
        std::string tmp = get(); // Line C2
        queueMutex.post();       // Line C2.5 (unlock)
        empty.post();            // Line C3
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
    // Start here work out how we will check that there aer more lines to places in buffer
    // whether we use put func or place logic here directly
    std::string line;

    while (true)
    {
        fileLock.wait();
        if (!std::getline(*file, line))
        {
            fileLock.post();
            break;
        }
        fileLock.post();

        empty.wait();      // Line P1
        queueMutex.wait(); // Line P1.5 (lock)
        put(line);         // Line P2
        queueMutex.post(); // Line P2.5 (unlock)
        full.post();       // Line P3
    }

    return nullptr;
}

int main(int argc, char *argv[])
{
    // fix-me:  check if source file exists...

    std::string numThreadsStr = argv[1];
    int threads = 0;
    if (argc != 4)
    {
        std::cerr << "Error: requires three arguements\n";
        return 1;
    }
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

    std::ifstream *file = new std::ifstream(sourcefile);
    if (!file->is_open())
    {
        std::cerr << "File not found" << std::endl;
    }

    pthread_t producerThreads[threads];
    pthread_t consumerThreads[threads];

    for (int i = 0; i < threads; i++)
    {
        // std::string name = source + std::to_string(i + 1) + txt;
        // *file is created on the heap so it lives no past the loop iteration
        // fileData *file = new fileData(name, sourceDir, destDir); // char arrays are implcitly changed to std::string
        pthread_create(&producerThreads[i], nullptr, producer, file);
        // pthread_create(&consumerThreads[i], nullptr, consumer, destFile);
    }
    for (int i = 0; i < threads; i++)
    {
        pthread_join(producerThreads[i], nullptr);
    }

    for (int i = 0; i < MAX; i++)
    {
        std::cout << "buffer at " << i << " is " << buffer[i] << "\n"
                  << std::endl;
    }

    // pthread_join(consumerThread, nullptr);

    return 0;
}

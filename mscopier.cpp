#include <iostream>
#include <pthread.h>
#include <fstream>
#include "zem.h"

// FIX-ME: must check return type of all api functions
// FIX-ME: check memory usage wuth valgribnd on titan
// FIX-ME: check if can run on teaching servers
// FIX-ME: create a make file to create your file

const int MAX = 20;
Zem empty(MAX);
Zem full(0);
Zem queueMutex(1);
Zem fileLock(1);
Zem writeMutex(1);

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
    std::cout << "----------"
              << std::endl;
    std::cout << "Buffer is:"
              << std::endl;
    for (int i = 0; i < MAX; i++)
    {
        std::cout << "Buffer " << i << " is: " << buffer[i] << std::endl;
    }
    std::cout << "Removed: " << tmp
              << std::endl;
    std::cout << "----------\n"
              << std::endl;
    return tmp;
}

void *consumer(void *arg)
{
    std::ofstream *outfile = static_cast<std::ofstream *>(arg);
    while (true)
    {
        full.wait();
        queueMutex.wait();
        std::string line = get();
        // std::cout << "Consumed: '" << line << "' from index " << use - 1 << std::endl;

        if (line == "__STOP__")
        {
            queueMutex.post();
            empty.post();
            break;
        }

        *outfile << line << "\n";
        queueMutex.post();
        empty.post();

        writeMutex.wait();

        writeMutex.post();
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
        std::cout << "Produced: '" << line << "' at index " << fill << std::endl;
        put(line);
        queueMutex.post();
        full.post();
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
            std::cerr << "Error: number of threads must be between 2 & 10\n";
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

    char *sourcefile = argv[2];
    char *destFile = argv[3];

    std::ifstream *file = new std::ifstream(sourcefile);
    if (!file->is_open())
    {
        std::cerr << "File not found" << std::endl;
        return 1;
    }

    std::ofstream *outfile = new std::ofstream(destFile, std::ios::out);
    if (!outfile->is_open())
    {
        std::cerr << "Could not open destination file!" << std::endl;
        return 1;
    }

    pthread_t producerThreads[threads];
    pthread_t consumerThreads[threads];
    int returnProducer;
    int returnConsumer;

    for (int i = 0; i < threads; i++)
    {
        returnProducer = pthread_create(&producerThreads[i], nullptr, producer, file);
        if (returnProducer != 0)
        {
            std::cerr << "Error when making producer thread" << std::endl;
        }

        returnConsumer = pthread_create(&consumerThreads[i], nullptr, consumer, outfile);
        if (returnConsumer != 0)
        {
            std::cerr << "Error when making consumer thread" << std::endl;
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
    file->close();

    for (int i = 0; i < threads; i++)
    {
        returnProducer = pthread_join(consumerThreads[i], nullptr);
        if (returnProducer != 0)
        {
            std::cerr << "Error when joining consumer thread" << std::endl;
            return 1;
        }
    }
    outfile->close();

    return 0;
}

#include <iostream>
#include <pthread.h>

void *printMessage(void *arg)
{
    char *message = (char *)(arg);
    std::cout << message << "\n"
              << std::endl;
    return nullptr;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Error: requires three arguements\n";
        return 1;
    }
    std::string numFilesStr = argv[1];
    int numFiles = 0;

    try
    {
        numFiles = std::stoi(numFilesStr);
        if (numFiles < 2 || numFiles > 10)
        {
            std::cerr << "Error: number of files must be between 2 & 10\n";
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
    }

    char *sourceDir = argv[2];
    char *destDir = argv[3];

    pthread_t threads[numFiles];

    pthread_create(&thread1, nullptr, printMessage, (void *)sourceDir);

    pthread_join(thread1, nullptr);

    std::cout << "Main thread exiting." << std::endl;
    return 0;
}
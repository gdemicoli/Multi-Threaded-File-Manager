#include "zem.h"
#include <stdexcept>
// semaphore class represented as Zem using cond variables and locks

Zem::Zem(int initialValue)
{
    value = initialValue;
    if (pthread_mutex_init(&lock, nullptr) != 0)
    {
        throw std::runtime_error("Mutex init failed");
    }
    if (pthread_cond_init(&cond, nullptr) != 0)
    {
        throw std::runtime_error("Cond init failed");
    }
}

Zem::~Zem()
{
    int returnVal;
    returnVal = pthread_mutex_destroy(&lock);
    if (returnVal != 0)
    {
        std::cerr << "Mutex destroy failed" << std::endl;
    }

    returnVal = pthread_cond_destroy(&cond);
    if (returnVal != 0)
    {
        std::cerr << "Cond destroy failed" << std::endl;
    }
}

void Zem::wait()
{
    int returnVal;

    returnVal = pthread_mutex_lock(&lock);
    if (returnVal != 0)
    {
        throw std::runtime_error("Mutex lock failed");
    }

    while (value <= 0)
    {
        returnVal = pthread_cond_wait(&cond, &lock);
        if (returnVal != 0)
        {
            throw std::runtime_error("Cond wait failed");
        }
    }
    value--;
    returnVal = pthread_mutex_unlock(&lock);
    if (returnVal != 0)
    {
        throw std::runtime_error("Mutex unlock failed");
    }
}

void Zem::post()
{
    int returnVal;

    returnVal = pthread_mutex_lock(&lock);

    if (returnVal != 0)
    {
        throw std::runtime_error("Mutex lock failed");
    }

    value++;
    returnVal = pthread_cond_signal(&cond);
    if (returnVal != 0)
    {
        throw std::runtime_error("Cond signal failed");
    }

    returnVal = pthread_mutex_unlock(&lock);
    if (returnVal != 0)
    {
        throw std::runtime_error("Mutex unlock failed");
    }
}

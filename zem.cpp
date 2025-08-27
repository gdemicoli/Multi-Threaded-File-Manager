#include "zem.h"
#include <stdexcept>

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
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void Zem::wait()
{
    pthread_mutex_lock(&lock);
    while (value <= 0)
    {
        pthread_cond_wait(&cond, &lock);
    }
    value--;
    pthread_mutex_unlock(&lock);
}

void Zem::post()
{
    pthread_mutex_lock(&lock);
    value++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

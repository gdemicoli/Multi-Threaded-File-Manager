#include <iostream>
#include <pthread.h>

void *printMessage(void *arg)
{
    char *message = (char *)(arg);
    std::cout << message << std::endl;
    return nullptr;
}

int main()
{
    pthread_t thread1, thread2;

    const char *msg1 = "Hello from thread 1";
    const char *msg2 = "Hello from thread 2";

    pthread_create(&thread1, nullptr, printMessage, (void *)msg1);
    pthread_create(&thread2, nullptr, printMessage, (void *)msg2);

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    std::cout << "Main thread exiting." << std::endl;
    return 0;
}
#ifndef ZEM_H
#define ZEM_H

#include <pthread.h>
#include <iostream>

class Zem
{
public:
    explicit Zem(int initialValue);
    ~Zem();

    void wait();
    void post();

    int getValue() const;
    void setValue(int val);

private:
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
};

#endif

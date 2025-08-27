#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include "common_threads.h"
#include "zem.h"

class rwLock
{
private:
    Zem lock;
    Zem writeLock;
    int readers;

public:
    rwLock(int r);
    ~rwLock();
    void acquireReadLock();
    void releaseReadLock();
    void acquireWriteLock();
    void releaseWriteLock();
};

rwLock::rwLock(int r) : readers(r), lock(1), writeLock(1) {}
rwLock::~rwLock() {}
void rwLock::acquireReadLock()
{
    lock.wait();
    readers++;
    if (readers == 1)
    {
        writeLock.wait();
    }
    lock.post();
}

void rwLock::releaseReadLock()
{
    lock.wait();
    readers--;
    if (readers == 0)
    {
        writeLock.post();
    }
    lock.post();
}

void rwLock::acquireWriteLock()
{
    writeLock.wait();
}

void

    void
    rwlock_release_writelock(rwlock_t *rw)
{
    Zem_post(&rw->writelock);
}

//
// Don't change the code below (just use it!)
//

int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_readlock(&lock);
        printf("read %d\n", value);
        rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_writelock(&lock);
        value++;
        printf("write %d\n", value);
        rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
        Pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
        Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <pthread.h>

class ThreadPool {
public:
    ThreadPool(int size);
    ~ThreadPool();
    void addTask(void* (*taskFunction)(void*), void* arg);
    void shutdown();

private:
    int size;
    std::queue<pthread_t> threads;
    pthread_mutex_t queueMutex;
    pthread_cond_t queueCond;
    bool running;

    static void* threadFunction(void* arg);
};

#endif

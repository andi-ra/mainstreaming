#include "thread_pool.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>

ThreadPool::ThreadPool(int size) : size(size), running(true) {
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&queueCond, NULL);
    for (int i = 0; i < size; ++i) {
        pthread_t thread;
        pthread_create(&thread, NULL, ThreadPool::threadFunction, this);
        threads.push(thread);
    }
    Logger::getInstance().log("Thread pool created");
}

ThreadPool::~ThreadPool() {
    shutdown();
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueCond);
}

void ThreadPool::addTask(void* (*taskFunction)(void*), void* arg) {
    pthread_mutex_lock(&queueMutex);
    pthread_t thread;
    pthread_create(&thread, NULL, taskFunction, arg);
    threads.push(thread);
    pthread_cond_signal(&queueCond);
    pthread_mutex_unlock(&queueMutex);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Task added to thread pool, thread ID: %lu", thread);
    Logger::getInstance().log(logMessage);
}

void ThreadPool::shutdown() {
    pthread_mutex_lock(&queueMutex);
    running = false;
    pthread_cond_broadcast(&queueCond);
    while (!threads.empty()) {
        pthread_join(threads.front(), NULL);
        threads.pop();
    }
    pthread_mutex_unlock(&queueMutex);
    Logger::getInstance().log("Thread pool shut down");
}

void* ThreadPool::threadFunction(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (pool->running) {
        pthread_mutex_lock(&pool->queueMutex);
        while (pool->running && pool->threads.empty()) {
            pthread_cond_wait(&pool->queueCond, &pool->queueMutex);
        }
        if (!pool->running) {
            pthread_mutex_unlock(&pool->queueMutex);
            break;
        }
        pthread_t thread = pool->threads.front();
        pool->threads.pop();
        pthread_mutex_unlock(&pool->queueMutex);
        pthread_join(thread, NULL);
    }
    return NULL;
}

#ifndef LOGGER_H
#define LOGGER_H

#include <queue>
#include <pthread.h>

class Logger {
public:
    static Logger& getInstance();
    void log(const char* message);
    void logClient(const char* message);
    void start();
    void stop();

private:
    Logger();
    ~Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    std::queue<const char*> logQueue;
    std::queue<const char*> clientLogQueue;
    pthread_mutex_t logMutex;
    pthread_mutex_t clientLogMutex;
    pthread_cond_t logCond;
    pthread_cond_t clientLogCond;
    bool running;
    pthread_t logThread;
    pthread_t clientLogThread;

    static void* logThreadFunction(void* arg);
    static void* clientLogThreadFunction(void* arg);
};

#endif

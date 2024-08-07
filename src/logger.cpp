#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Logger::Logger() {
    pthread_mutex_init(&logMutex, NULL);
    pthread_mutex_init(&clientLogMutex, NULL);
    pthread_cond_init(&logCond, NULL);
    pthread_cond_init(&clientLogCond, NULL);
    running = true;
    pthread_create(&logThread, NULL, Logger::logThreadFunction, this);
    pthread_create(&clientLogThread, NULL, Logger::clientLogThreadFunction, this);
}

Logger::~Logger() {
    pthread_mutex_destroy(&logMutex);
    pthread_mutex_destroy(&clientLogMutex);
    pthread_cond_destroy(&logCond);
    pthread_cond_destroy(&clientLogCond);
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const char* message) {
    pthread_mutex_lock(&logMutex);
    logQueue.push(strdup(message));
    pthread_cond_signal(&logCond);
    pthread_mutex_unlock(&logMutex);
}

void Logger::logClient(const char* message) {
    pthread_mutex_lock(&clientLogMutex);
    clientLogQueue.push(strdup(message));
    pthread_cond_signal(&clientLogCond);
    pthread_mutex_unlock(&clientLogMutex);
}

void Logger::start() {
    running = true;
}

void Logger::stop() {
    running = false;
    pthread_cond_signal(&logCond);
    pthread_cond_signal(&clientLogCond);
    pthread_join(logThread, NULL);
    pthread_join(clientLogThread, NULL);
}

void* Logger::logThreadFunction(void* arg) {
    Logger* logger = static_cast<Logger*>(arg);
    FILE* logFile = fopen("logs/server.log", "a");

    while (logger->running || !logger->logQueue.empty()) {
        pthread_mutex_lock(&logger->logMutex);
        while (logger->logQueue.empty()) {
            pthread_cond_wait(&logger->logCond, &logger->logMutex);
        }

        while (!logger->logQueue.empty()) {
            const char* message = logger->logQueue.front();
            logger->logQueue.pop();
            fprintf(logFile, "%s\n", message);
            fflush(logFile);
            free((void*)message);
        }

        pthread_mutex_unlock(&logger->logMutex);
    }

    fclose(logFile);
    return NULL;
}

void* Logger::clientLogThreadFunction(void* arg) {
    Logger* logger = static_cast<Logger*>(arg);
    FILE* logFile = fopen("logs/client.log", "a");

    while (logger->running || !logger->clientLogQueue.empty()) {
        pthread_mutex_lock(&logger->clientLogMutex);
        while (logger->clientLogQueue.empty()) {
            pthread_cond_wait(&logger->clientLogCond, &logger->clientLogMutex);
        }

        while (!logger->clientLogQueue.empty()) {
            const char* message = logger->clientLogQueue.front();
            logger->clientLogQueue.pop();
            fprintf(logFile, "%s\n", message);
            fflush(logFile);
            free((void*)message);
        }

        pthread_mutex_unlock(&logger->clientLogMutex);
    }

    fclose(logFile);
    return NULL;
}

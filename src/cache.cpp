#include "cache.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CHUNK_SIZE 4096  // Size of file chunks to read and send

Cache& Cache::getInstance() {
    static Cache instance;
    return instance;
}

Cache::Cache() {
    pthread_mutex_init(&cacheMutex, NULL);
}

void Cache::init(int ttl) {
    this->ttl = ttl;
    Logger::getInstance().log("Cache initialized");
}

const char* Cache::get(const char* key) {
    pthread_mutex_lock(&cacheMutex);
    std::map<std::string, CacheEntry*>::iterator it = cache.find(key);
    if (it != cache.end() && it->second->expireTime > time(NULL)) {
        Logger::getInstance().log("Cache hit");
        pthread_mutex_unlock(&cacheMutex);
        return it->second->data;
    }
    Logger::getInstance().log("Cache miss");
    pthread_mutex_unlock(&cacheMutex);
    return NULL;
}

void Cache::put(const char* key, const char* data) {
    pthread_mutex_lock(&cacheMutex);
    expire(key);  // Remove existing entry if any
    
    CacheEntry* entry = new CacheEntry;
    entry->data = strdup(data);
    entry->expireTime = time(NULL) + ttl;
    cache[key] = entry;
    Logger::getInstance().log("Cache put");
    pthread_mutex_unlock(&cacheMutex);
}

void Cache::expire(const char* key) {
    std::map<std::string, CacheEntry*>::iterator it = cache.find(key);
    if (it != cache.end()) {
        free(it->second->data);
        delete it->second;
        cache.erase(it);
        Logger::getInstance().log("Cache entry expired");
    }
}

bool Cache::fetchAndCacheFile(const char* path, std::string& fileContent) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        Logger::getInstance().log("Failed to open file for reading");
        return false;
    }

    char buffer[CHUNK_SIZE];
    int bytesRead;
    int chunkCount = 0;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        fileContent.append(buffer, bytesRead);
        chunkCount++;
    }

    close(fd);

    if (bytesRead < 0) {
        Logger::getInstance().log("Failed to read file completely");
        return false;
    }

    char logMessage[128];
    snprintf(logMessage, sizeof(logMessage), "File read completely, chunks read: %d", chunkCount);
    Logger::getInstance().log(logMessage);
    put(path, fileContent.c_str());

    return true;
}

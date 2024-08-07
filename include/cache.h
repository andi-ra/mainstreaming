#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <string>
#include <time.h>
#include <pthread.h>

struct CacheEntry {
    char* data;
    time_t expireTime;
};

class Cache {
public:
    static Cache& getInstance();
    void init(int ttl);
    const char* get(const char* key);
    void put(const char* key, const char* data);
    void expire(const char* key);
    bool fetchAndCacheFile(const char* path, std::string& fileContent);

private:
    Cache();
    Cache(const Cache&);
    Cache& operator=(const Cache&);

    int ttl;
    std::map<std::string, CacheEntry*> cache;
    pthread_mutex_t cacheMutex;
};

#endif

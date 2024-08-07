#include "config.h"
#include "logger.h"
#include "cache.h"
#include "server.h"
#include "socket.h"
#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ConfigManager& configManager = ConfigManager::getInstance();
    configManager.loadConfig(argv[1]);
    const Config& config = configManager.getConfig();

    Logger::getInstance().start();
    Logger::getInstance().log("Server starting");

    Cache& cache = Cache::getInstance();
    cache.init(config.cache_ttl);

    ThreadPool* threadPool = new ThreadPool(10);
    Socket* socket = SocketFactory::createSocket(true, configManager);

    Server* server = new Server(socket, &cache, threadPool);
    server->start();

    Logger::getInstance().log("Server stopped");
    Logger::getInstance().stop();
    delete server;
    delete socket;
    delete threadPool;

    return 0;
}

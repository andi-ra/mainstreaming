#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "cache.h"
#include "logger.h"
#include "thread_pool.h"

class Server;

struct ClientRequest {
    Server* server;
    int clientSock;
};

class Server {
public:
    Server(Socket* socket, Cache* cache, ThreadPool* threadPool);
    void start();
    void handleRequest(int clientSock);

private:
    Socket* socket;
    Cache* cache;
    ThreadPool* threadPool;
};

#endif

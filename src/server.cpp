#include "server.h"
#include "logger.h"
#include "socket.h"
#include "cache.h"
#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_ACCEPT_RETRIES 5
#define INITIAL_ACCEPT_RETRY_DELAY 1  // in seconds
#define CHUNK_SIZE 4096  // Size of file chunks to read and send

void* handleRequestWrapper(void* arg);

Server::Server(Socket* socket, Cache* cache, ThreadPool* threadPool)
    : socket(socket), cache(cache), threadPool(threadPool) {
    Logger::getInstance().log("Server instance created");
}

void Server::start() {
    if (socket->bind(8080) == 0) {
        Logger::getInstance().log("Socket successfully bound to port 8080");
    } else {
        Logger::getInstance().log("Failed to bind socket to port 8080");
        return;
    }

    if (socket->listen(10) == 0) {
        Logger::getInstance().log("Socket is listening on port 8080");
    } else {
        Logger::getInstance().log("Failed to listen on socket");
        return;
    }

    int acceptRetries = 0;
    int acceptRetryDelay = INITIAL_ACCEPT_RETRY_DELAY;

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    while (true) {
        int clientSock = socket->accept((struct sockaddr *)&address, &addrlen);
        if (clientSock == -1) {
            if (++acceptRetries >= MAX_ACCEPT_RETRIES) {
                Logger::getInstance().log("Max retries reached: Unable to accept client connection. Pausing before retrying.");
                sleep(acceptRetryDelay);
                acceptRetries = 0;
                acceptRetryDelay *= 2; // Exponential backoff
                if (acceptRetryDelay > 60) { // Cap the retry delay to 1 minute
                    acceptRetryDelay = 60;
                }
            } else {
                Logger::getInstance().log("Unable to accept client connection: Bad file descriptor. Retrying...");
            }
            continue; // Skip this iteration if accept failed
        }
        acceptRetries = 0; // Reset retries on successful accept
        acceptRetryDelay = INITIAL_ACCEPT_RETRY_DELAY; // Reset delay on successful accept

        // Wrap the client socket in a struct to pass to the thread function
        ClientRequest* clientRequest = new ClientRequest;
        clientRequest->server = this;
        clientRequest->clientSock = clientSock;

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handleRequestWrapper, clientRequest) != 0) {
            Logger::getInstance().log("Failed to create thread for handling client request");
            close(clientSock);
            delete clientRequest;
        } else {
            pthread_detach(clientThread); // Detach the thread so that it cleans up after itself
        }
    }
}

void* handleRequestWrapper(void* arg) {
    ClientRequest* clientRequest = (ClientRequest*)arg;
    clientRequest->server->handleRequest(clientRequest->clientSock);
    delete clientRequest;
    return NULL;
}

void Server::handleRequest(int clientSock) {
    Logger::getInstance().log("Handling new client request");
    char buffer[1024];
    int bytesRead = socket->read(clientSock, buffer, sizeof(buffer));
    Logger::getInstance().log("Read request from client socket");

    if (bytesRead <= 0) {
        Logger::getInstance().log("Failed to read from socket or connection closed");
        close(clientSock);
        return;
    }

    char method[16], path[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    if (strcmp(method, "GET") == 0) {
        const char* data = cache->get(path);
        if (data) {
            Logger::getInstance().log("Cache hit: Serving from cache");
            socket->write(clientSock, "HTTP/1.1 200 OK\r\n\r\n", 19);
            socket->write(clientSock, data, strlen(data));
        } else {
            Logger::getInstance().log("Cache miss: Fetching from file system");
            std::string fileContent;
            char filePath[512];
            snprintf(filePath, sizeof(filePath), "content%s", path);
            if (cache->fetchAndCacheFile(filePath, fileContent)) {
                socket->write(clientSock, "HTTP/1.1 200 OK\r\n\r\n", 19);
                socket->write(clientSock, fileContent.c_str(), fileContent.size());
                Logger::getInstance().log("File served from filesystem");
            } else {
                socket->write(clientSock, "HTTP/1.1 404 Not Found\r\n\r\n", 26);
                Logger::getInstance().log("File not found");
            }
        }
        Logger::getInstance().log("Request handling finished");
    }

    close(clientSock);
    Logger::getInstance().log("Client socket closed");
}

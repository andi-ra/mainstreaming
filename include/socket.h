#ifndef SOCKET_H
#define SOCKET_H

#include <openssl/ssl.h>
#include <netinet/in.h>

class ConfigManager;

class Socket {
public:
    virtual ~Socket() {}
    virtual int bind(int port) = 0;
    virtual int listen(int backlog) = 0;
    virtual int accept(struct sockaddr *addr, socklen_t *addrlen) = 0;
    virtual int read(int clientSock, char* buffer, int size) = 0;
    virtual int write(int clientSock, const char* buffer, int size) = 0;
    virtual void close() = 0;
};

class TCPSocket : public Socket {
public:
    TCPSocket();
    ~TCPSocket();
    int bind(int port);
    int listen(int backlog);
    int accept(struct sockaddr *addr, socklen_t *addrlen);
    int read(int clientSock, char* buffer, int size);
    int write(int clientSock, const char* buffer, int size);
    void close();

private:
    int sockfd;
};

class SSLSocket : public Socket {
public:
    SSLSocket(const char* certs_path, const char* key_path);
    ~SSLSocket();
    int bind(int port);
    int listen(int backlog);
    int accept(struct sockaddr *addr, socklen_t *addrlen);
    int read(int clientSock, char* buffer, int size);
    int write(int clientSock, const char* buffer, int size);
    void close();

private:
    int sockfd;
    SSL_CTX* ctx;
    SSL* ssl;
};

class SocketFactory {
public:
    static Socket* createSocket(bool use_ssl, const ConfigManager& configManager);
};

#endif

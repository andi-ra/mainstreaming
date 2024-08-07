#include "socket.h"
#include "config.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

TCPSocket::TCPSocket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Unable to create socket");
        Logger::getInstance().log("Failed to create TCP socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("TCP socket created");
}

TCPSocket::~TCPSocket() {
    close();
}

int TCPSocket::bind(int port) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to bind socket");
        Logger::getInstance().log("Failed to bind TCP socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("TCP socket bound to port");
    return 0;
}

int TCPSocket::listen(int backlog) {
    if (::listen(sockfd, backlog) < 0) {
        perror("Unable to listen on socket");
        Logger::getInstance().log("Failed to listen on TCP socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("Listening on TCP socket");
    return 0;
}

int TCPSocket::accept(struct sockaddr *addr, socklen_t *addrlen) {
    int client_sockfd = ::accept(sockfd, addr, addrlen);
    if (client_sockfd < 0) {
        perror("Unable to accept client connection");
        Logger::getInstance().log("Failed to accept client connection on TCP socket");
    } else {
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Accepted client connection from %s", inet_ntoa(((struct sockaddr_in*)addr)->sin_addr));
        Logger::getInstance().log(logMessage);
    }
    return client_sockfd;
}

int TCPSocket::read(int clientSock, char* buffer, int size) {
    int bytes_read = ::read(clientSock, buffer, size);
    if (bytes_read < 0) {
        Logger::getInstance().log("Failed to read from TCP socket");
    } else {
        Logger::getInstance().log("Read from TCP socket");
    }
    return bytes_read;
}

int TCPSocket::write(int clientSock, const char* buffer, int size) {
    int bytes_written = ::write(clientSock, buffer, size);
    if (bytes_written < 0) {
        Logger::getInstance().log("Failed to write to TCP socket");
    } else {
        Logger::getInstance().log("Written to TCP socket");
    }
    return bytes_written;
}

void TCPSocket::close() {
    ::close(sockfd);
    Logger::getInstance().log("TCP socket closed");
}

SSLSocket::SSLSocket(const char* certs_path, const char* key_path) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());

    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        Logger::getInstance().log("Failed to create SSL context");
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_certificate_file(ctx, certs_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        Logger::getInstance().log("Failed to load SSL certificate");
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        Logger::getInstance().log("Failed to load SSL private key");
        exit(EXIT_FAILURE);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        Logger::getInstance().log("SSL private key does not match the certificate public key");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Unable to create socket");
        Logger::getInstance().log("Failed to create SSL socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("SSL socket created");
}

SSLSocket::~SSLSocket() {
    SSL_CTX_free(ctx);
    close();
}

int SSLSocket::bind(int port) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to bind socket");
        Logger::getInstance().log("Failed to bind SSL socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("SSL socket bound to port");
    return 0;
}

int SSLSocket::listen(int backlog) {
    if (::listen(sockfd, backlog) < 0) {
        perror("Unable to listen on socket");
        Logger::getInstance().log("Failed to listen on SSL socket");
        exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("Listening on SSL socket");
    return 0;
}

int SSLSocket::accept(struct sockaddr *addr, socklen_t *addrlen) {
    int client_sockfd = ::accept(sockfd, addr, addrlen);
    if (client_sockfd < 0) {
        perror("Unable to accept client connection");
        Logger::getInstance().log("Failed to accept client connection on SSL socket");
    } else {
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sockfd);
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            Logger::getInstance().log("Failed to accept SSL connection");
            ::close(client_sockfd);
            client_sockfd = -1;
        } else {
            char logMessage[256];
            snprintf(logMessage, sizeof(logMessage), "Accepted client connection from %s", inet_ntoa(((struct sockaddr_in*)addr)->sin_addr));
            Logger::getInstance().log(logMessage);
        }
    }
    return client_sockfd;
}

int SSLSocket::read(int clientSock, char* buffer, int size) {
    int bytes_read = SSL_read(ssl, buffer, size);
    if (bytes_read < 0) {
        Logger::getInstance().log("Failed to read from SSL socket");
    } else {
        Logger::getInstance().log("Read from SSL socket");
    }
    return bytes_read;
}

int SSLSocket::write(int clientSock, const char* buffer, int size) {
    int bytes_written = SSL_write(ssl, buffer, size);
    if (bytes_written < 0) {
        Logger::getInstance().log("Failed to write to SSL socket");
    } else {
        Logger::getInstance().log("Written to SSL socket");
    }
    return bytes_written;
}

void SSLSocket::close() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl = NULL;
    }
    if (sockfd >= 0) {
        ::close(sockfd);
        sockfd = -1;
    }
    Logger::getInstance().log("SSL socket closed");
}

Socket* SocketFactory::createSocket(bool use_ssl, const ConfigManager& configManager) {
    const Config& config = configManager.getConfig();
    if (use_ssl) {
        return new SSLSocket(config.certs_path, config.key_path);
    } else {
        return new TCPSocket();
    }
}

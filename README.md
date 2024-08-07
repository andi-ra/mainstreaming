# H1Web Server

This project implements a multi-threaded web server in C++98 that supports both HTTP and HTTPS. The server reads configuration from a JSON file and uses a time-expiry cache to serve files efficiently. Detailed logging is implemented using a producer-consumer pattern.
Features

1. Multi-threaded server using a thread pool.
2. Supports HTTP and HTTPS.
3. Reads configuration from a JSON file.
4. In-memory cache with TTL (time-to-live) for cached items.
5. Detailed logging of server actions and client requests.

# H2 Project Structure

    include/: Header files for various modules.
    src/: Source files for various modules.
    lib/: Compiled libraries.
    bin/: Compiled executable.
    config/: Configuration files.
    logs/: Log files.
    content/: Directory for content to be served.

# H3 Modules
# H4Server Module

Handles incoming client connections, dispatches them to worker threads, and processes HTTP/HTTPS requests.
Logger Module
# H4 Logger Module
Implements logging using a producer-consumer pattern. Logs are written to files in the background by a separate thread.

# H4 Cache Module
Implements an in-memory cache with time-to-live (TTL) for cached items. Supports fetching files and caching them in chunks.

# H4 Socket Module
Provides an abstraction for TCP and SSL sockets, using the Bridge design pattern to support both types of connections.

 # H4 Config Manager Module

Reads configuration from a JSON file and provides configuration parameters to other modules. Implemented as a singleton.
Compilation and Running Instructions
Prerequisites

    C++ compatible compiler (e.g., g++).
    OpenSSL library for HTTPS support.
    jsoncpp library for JSON parsing.

# H3 Compilation
    cd mainstreaming
    make
    ./bin/server config.json 
# H3 Clone the repository:

    sh
    https://github.com/andi-ra/mainstreaming.git
    cd mainstreaming

# H3 Configuration

Edit the config/config.json file to set the following parameters:

    port: The port on which the server will listen.
    certs_path: Path to the SSL certificate file.
    key_path: Path to the SSL key file.
    log_path: Path to the log files.
    content_directory: Directory for content to be served.
    cache_ttl: Time-to-live for cached items in seconds.

Example config/config.json:

    json

    {
        "port": 8080,
        "certs_path": "config/server.crt",
        "key_path": "config/server.key",
        "log_path": "logs/",
        "content_directory": "content/",
        "cache_ttl": 3600
    }

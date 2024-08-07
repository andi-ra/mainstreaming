Web Server

This project implements a multi-threaded web server in C++98 that supports both HTTP and HTTPS. The server reads configuration from a JSON file and uses a time-expiry cache to serve files efficiently. Detailed logging is implemented using a producer-consumer pattern.
Features

    Multi-threaded server using a thread pool.
    Supports HTTP and HTTPS.
    Reads configuration from a JSON file.
    In-memory cache with TTL (time-to-live) for cached items.
    Detailed logging of server actions and client requests.

Project Structure

    include/: Header files for various modules.
    src/: Source files for various modules.
    lib/: Compiled libraries.
    bin/: Compiled executable.
    config/: Configuration files.
    logs/: Log files.
    content/: Directory for content to be served.

Modules
Server Module

Handles incoming client connections, dispatches them to worker threads, and processes HTTP/HTTPS requests.
Logger Module

Implements logging using a producer-consumer pattern. Logs are written to files in the background by a separate thread.
Cache Module

Implements an in-memory cache with time-to-live (TTL) for cached items. Supports fetching files and caching them in chunks.
Socket Module

Provides an abstraction for TCP and SSL sockets, using the Bridge design pattern to support both types of connections.
Config Manager Module

Reads configuration from a JSON file and provides configuration parameters to other modules. Implemented as a singleton.
Compilation and Running Instructions
Prerequisites

    C++ compatible compiler (e.g., g++).
    OpenSSL library for HTTPS support.
    jsoncpp library for JSON parsing.

Compilation

    Clone the repository:

    sh

git clone https://github.com/your-username/your-repository-name.git
cd your-repository-name

Set up the project:

sh

./setup.sh

Compile the project:

sh

    make all

Running the Server

    Run the server with HTTP support:

    sh

./bin/server config/config.json

Run the server with HTTPS support:

sh

    ./bin/server config/config.json --use-ssl

Configuration

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

Logging

Logs are written to the logs/ directory. There are separate log files for server actions and client requests.
Contributing

    Fork the repository.
    Create a new branch (git checkout -b feature-branch).
    Commit your changes (git commit -am 'Add new feature').
    Push to the branch (git push origin feature-branch).
    Create a new Pull Request.

License

This project is licensed under the MIT License.

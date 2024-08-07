#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jsoncpp/json/json.h>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open config file");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    size_t readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[readSize] = '\0'; // Null terminate the buffer
    fclose(file);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(buffer, root)) {
        fprintf(stderr, "Failed to parse config file: %s\n", reader.getFormattedErrorMessages().c_str());
        exit(EXIT_FAILURE);
    }

    if (root.isMember("port") && root["port"].isInt()) {
        config.port = root["port"].asInt();
    } else {
        fprintf(stderr, "Config error: 'port' is missing or not an integer\n");
        exit(EXIT_FAILURE);
    }

    if (root.isMember("cache_ttl") && root["cache_ttl"].isInt()) {
        config.cache_ttl = root["cache_ttl"].asInt();
    } else {
        fprintf(stderr, "Config error: 'cache_ttl' is missing or not an integer\n");
        exit(EXIT_FAILURE);
    }

    if (root.isMember("content_path") && root["content_path"].isString()) {
        strncpy(config.content_path, root["content_path"].asCString(), sizeof(config.content_path));
    } else {
        fprintf(stderr, "Config error: 'content_path' is missing or not a string\n");
        exit(EXIT_FAILURE);
    }

    if (root.isMember("certs_path") && root["certs_path"].isString()) {
        strncpy(config.certs_path, root["certs_path"].asCString(), sizeof(config.certs_path));
    } else {
        fprintf(stderr, "Config error: 'certs_path' is missing or not a string\n");
        exit(EXIT_FAILURE);
    }

    if (root.isMember("key_path") && root["key_path"].isString()) {
        strncpy(config.key_path, root["key_path"].asCString(), sizeof(config.key_path));
    } else {
        fprintf(stderr, "Config error: 'key_path' is missing or not a string\n");
        exit(EXIT_FAILURE);
    }

    if (root.isMember("log_path") && root["log_path"].isString()) {
        strncpy(config.log_path, root["log_path"].asCString(), sizeof(config.log_path));
    } else {
        fprintf(stderr, "Config error: 'log_path' is missing or not a string\n");
        exit(EXIT_FAILURE);
    }
}

const Config& ConfigManager::getConfig() const {
    return config;
}

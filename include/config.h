#ifndef CONFIG_H
#define CONFIG_H

struct Config {
    int port;
    int cache_ttl;
    char content_path[256];
    char certs_path[256];
    char key_path[256];
    char log_path[256];
};

class ConfigManager {
public:
    static ConfigManager& getInstance();
    void loadConfig(const char* filename);
    const Config& getConfig() const;

private:
    ConfigManager() {}
    ConfigManager(const ConfigManager&);
    ConfigManager& operator=(const ConfigManager&);

    Config config;
};

#endif

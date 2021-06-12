#include "config.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define CONFIG_FILENAME "/config.json"

Config::Config() {
    Serial.println("Config::Setup");
    this->read();
}

Config::~Config() {
    Serial.println("Config::Destroy");
    if (this->m_ntp_server != NULL) {
        free(this->m_ntp_server);
    }
}

void Config::reset() {
    File file = SPIFFS.open(CONFIG_FILENAME, FILE_WRITE);
    file.print("{\"mode\": \"setup\", \"ntp_server\": \"pool.ntp.org\", \"ntp_gmt_offset\": 0, \"ntp_daylight_offset\": 0}");
    file.close();
}

void Config::read() {
    StaticJsonDocument<2046> doc;
    DeserializationError error;
    File file;

    Serial.println("Config::Reading JSON config from Flash");
    file = SPIFFS.open(CONFIG_FILENAME, FILE_READ);
    if (!file) {
        Serial.println("Configuration file not found. Setting default config");
        this->reset();
        file = SPIFFS.open(CONFIG_FILENAME, FILE_READ);
    }

    Serial.println("Config::Decoding JSON config");
    error = deserializeJson(doc, file);
    if (error || !doc.containsKey("mode")){
        Serial.println("Configuration file is corrupted. Setting default config");
        this->reset();
        file = SPIFFS.open(CONFIG_FILENAME, FILE_READ);
        error = deserializeJson(doc, file);
    }
    file.close();

    if (error) {
        Serial.print(F("Config::deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Parse JSON configuration
    Serial.println("Config::Parsing JSON config");
    this->m_mode = this->decode_mode((const char *) doc["mode"]);
    this->m_ntp_server = this->decode_json_key_as_name(doc, "ntp_server", "pool.ntp.org");
    this->m_ntp_gmt_offset = this->decode_json_key_as_long(doc, "ntp_gmt_offset", 0);
    this->m_ntp_daylight_offset = this->decode_json_key_as_long(doc, "ntp_daylight_offset", 0);

}

MODE Config::get_mode() {
    return this->m_mode;
}

char *Config::get_ntp_server() {
    return this->m_ntp_server;
}

long int Config::get_ntp_gmt_offset() {
    return this->m_ntp_gmt_offset;
}

long int Config::get_ntp_daylight_offset() {
    return this->m_ntp_daylight_offset;
}

MODE Config::decode_mode(const char *mode_str) {
    if (strcmp(mode_str, "setup") == 0) {
        return MODE::SETUP;
    } else {
        return MODE::NORMAL;
    }
}

long int Config::decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value) {
    long value = default_value;
    if (doc.containsKey(key)) {
        value = (long) doc[key];
    }
    return value;
}

char *Config::decode_json_key_as_name(JsonDocument &doc, const char *key, const char *default_value) {
    char *name = NULL;
    if (doc.containsKey(key)) {
        name = this->decode_name((const char *)doc[key]);
    } else {
        name = this->decode_name(default_value);
    }
    return name;
}

char *Config::decode_name(const char *name_str) {
    char *name = (char *) malloc(sizeof(char) * strlen(name_str));
    strcpy(name, name_str);
    return name;
}


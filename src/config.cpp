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
    this->m_mode = this->decode_json_key_as_mode(doc, "mode", MODE::SETUP);
    this->m_ntp_server = this->decode_json_key_as_name(doc, "ntp_server", "pool.ntp.org");
    this->m_ntp_gmt_offset = this->decode_json_key_as_long(doc, "ntp_gmt_offset", 0);
    this->m_ntp_daylight_offset = this->decode_json_key_as_long(doc, "ntp_daylight_offset", 0);
    this->m_wifi_ssid = this->decode_json_key_as_name(doc, "wifi_ssid", nullptr);
    this->m_wifi_password = this->decode_json_key_as_name(doc, "wifi_password", nullptr);

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

const char *Config::get_wifi_ssid() {
    return static_cast<const char *> (this->m_wifi_ssid);
}

const char *Config::get_wifi_password() {
    return static_cast<const char *> (this->m_wifi_password);
}

MODE Config::decode_json_key_as_mode(JsonDocument &doc, const char *key, MODE default_value) {
    if (doc.containsKey(key) && doc[key].is<const char *>()) {
        if (strcmp(doc[key], "setup") == 0) {
            return MODE::SETUP;
        }
        else {
            return MODE::NORMAL;
        }
    }
    return default_value;
}

long int Config::decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value) {
    long value = default_value;
    if (doc.containsKey(key) && doc[key].is<long int>()) {
        value = (long int) doc[key];
    }
    return value;
}

char* Config::decode_json_key_as_name(JsonDocument &doc, const char *key, const char* default_value) {
    if (doc.containsKey(key) && doc[key].is<const char *>()) {
        return const_cast<char *>(doc[key].as<const char *>());
    }
    if (default_value != nullptr) {
        return const_cast<char *>(default_value);
    }
    return nullptr;
}

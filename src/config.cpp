#include "config.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>


Config::Config() {
    Serial.println("Config::Setup");
    this->read();
}

Config::~Config() {
    Serial.println("Config::Destroy");
}

void Config::reset() {
    File file = SPIFFS.open(CONFIG_FILENAME, FILE_WRITE);
    String default_values = R"(
        {
            "sensors": [
                { "type": "HDC2080", "value" : 64 },
                { "type": "HDC2080", "value" : 65 }
            ] ,
            "schedule" : [
                { "start": "00:00", "end" : "24:00", "value" : 10 }
            ] ,
            "mode" : "setup",
            "handler" : "test"
            "wifi_ssid" : "",
            "wifi_password" : "",
            "ntp_server" : "pool.ntp.org",
            "ntp_gmt_offset" : 2,
            "heat" :  { "type": "PWM", "value" : 16 },
            "cold" :  { "type": "RELAY", "value" : 19 },
            "fan" :   { "type": "RELAY", "value" : 18 },
            "light" : { "type": "NONE", "value" : 5 },
        }
    )";
    file.print(default_values);
    //file.print("{\"mode\": \"setup\", \"ntp_server\": \"pool.ntp.org\", \"ntp_gmt_offset\": 0, \"ntp_daylight_offset\": 0}");

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
    this->m_handler = this->decode_json_key_as_handler(doc, "handler", HANDLER::TEST);

    this->m_ntp_server = this->decode_json_key_as_name(doc, "ntp_server", "pool.ntp.org");
    this->m_ntp_gmt_offset = this->decode_json_key_as_long(doc, "ntp_gmt_offset", 0);
    this->m_ntp_daylight_offset = this->decode_json_key_as_long(doc, "ntp_daylight_offset", 0);
    this->m_wifi_ssid = this->decode_json_key_as_name(doc, "wifi_ssid", "");
    this->m_wifi_password = this->decode_json_key_as_name(doc, "wifi_password", "");

    for (JsonObject repo : doc["sensors"].as<JsonArray>()) {
        if (strcmp(repo["type"].as<const char*>(), "DS18B20") == 0) {
            this->sensors.push_back(new Sensor_DS18B20(repo["value"].as<int>()));
        }
        else if (strcmp(repo["type"].as<const char*>(), "HDC2080") == 0) {
            this->sensors.push_back(new Sensor_HDC2080(repo["value"].as<int>()));
        }
        else if (strcmp(repo["type"].as<const char*>(), "BMP280") == 0) {
            this->sensors.push_back(new Sensor_BMP280(repo["value"].as<int>()));
        }
        else if (strcmp(repo["type"].as<const char*>(), "DOOR") == 0) {
            this->sensors.push_back(new Sensor_DOOR(repo["value"].as<int>()));
        }
    }

    this->cold  = this->set_actuator(doc["cold"]["type"].as<const char*>(),doc["cold"]["value"].as<int>());
    this->heat  = this->set_actuator(doc["heat"]["type"].as<const char*>(),doc["heat"]["value"].as<int>());
    this->fan   = this->set_actuator(doc["fan"]["type"].as<const char*>(),doc["fan"]["value"].as<int>());
    this->light = this->set_actuator(doc["light"]["type"].as<const char*>(),doc["light"]["value"].as<int>());

    for (JsonObject repo : doc["schedule"].as<JsonArray>()) {
        this->schedule.push_back(new Timetable(repo["start"].as<const char*>(), repo["end"].as<const char*>(), repo["value"].as<float>()));
    }
}

Actuator* Config::set_actuator(String type, int value)
{
    if(type == "RELAY"){
        return new Relay(value);
    }
    else if (type == "SOLID") {
        return new Solid(value);
    }
    else if (type == "PWM") {
        return new SoftPwm(value);
    }

    return nullptr;
}

MODE Config::get_mode() {
    return this->m_mode;
}
HANDLER Config::get_handler(){
    return m_handler;
}

String Config::get_mode_string() {
    switch (m_mode)
    {
        case MODE::SETUP:
            return "setup";
        case MODE::NORMAL:
            return "normal";
    }
    return "unknow";
}

String Config::get_handler_string() {
    switch (m_handler)
    {
        case HANDLER::TEST:
            return "test";
        case HANDLER::BASIC:
            return "basic";
        case HANDLER::PID:
            return "pid";
    }
    return "unknow";
}

String Config::get_ntp_server() {
    return this->m_ntp_server;
}

long int Config::get_ntp_gmt_offset() {
    return this->m_ntp_gmt_offset;
}

long int Config::get_ntp_daylight_offset() {
    return this->m_ntp_daylight_offset;
}

String Config::get_wifi_ssid() {
    return this->m_wifi_ssid;
}

String Config::get_wifi_password() {
    return this->m_wifi_password;
}

float Config::get_temperature_reference(){
    for (auto timetable : this->schedule)
    {
        if(timetable->is_in_interval()){
            return timetable->get_value();
        }
    }
    //TODO send alert, error in schedule
    return TEMP_ERROR_READING;
}

MODE Config::decode_json_key_as_mode(JsonDocument &doc, const char *key, MODE default_value) {
    if (doc.containsKey(key) && doc[key].is<const char *>()) {
        if (strcmp(doc[key].as<const char*>(), "setup") == 0) {
            return MODE::SETUP;
        }
        else {
            return MODE::NORMAL;
        }
    }
    return default_value;
}

HANDLER Config::decode_json_key_as_handler(JsonDocument& doc, const char* key, HANDLER default_value) {
    if (doc.containsKey(key) && doc[key].is<const char*>()) {
        if (strcmp(doc[key].as<const char*>(), "test") == 0) {
            return HANDLER::TEST;
        }else if (strcmp(doc[key].as<const char*>(), "basic") == 0) {
            return HANDLER::BASIC;
        }else if (strcmp(doc[key].as<const char*>(), "pid") == 0) {
            return HANDLER::PID;
        }
    }
    return default_value;
}

long int Config::decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value) {
    long value = default_value;
    if (doc.containsKey(key) && doc[key].is<long int>()) {
        value = doc[key].as<long int>();
    }
    return value;
}

String Config::decode_json_key_as_name(JsonDocument &doc, const char *key, const char* default_value) {
    if (doc.containsKey(key) && doc[key].is<const char *>()) {
        return doc[key].as<const char *>();
    }
    if (strcmp(default_value, "") != 0) {
        return default_value;
    }
    return "";
}

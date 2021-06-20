#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include "sensor.h"
#include "actuator.h"

#define CONFIG_FILENAME "/config.json"

enum class MODE { SETUP = 0, NORMAL };

class Config {
public:
    Config();
    ~Config();
    void reset();
    void read();
    MODE get_mode();
    String get_ntp_server();
    long int get_ntp_gmt_offset();
    long int get_ntp_daylight_offset();
    String get_wifi_ssid();
    String get_wifi_password();

    std::vector<Sensor*> sensors;
    Sensor_DOOR *door = nullptr;
    Sensor_BUTTON *button = nullptr;

    Relay* cold = nullptr;
    Relay* heat = nullptr;
    Relay* fan = nullptr;
    Relay* light = nullptr;

private:
    MODE m_mode = MODE::SETUP;

    String m_ntp_server = "";

    long int m_ntp_gmt_offset = 0;
    long int m_ntp_daylight_offset = 0;

    String m_wifi_ssid = "";
    String m_wifi_password = "";

    MODE decode_json_key_as_mode(JsonDocument &doc, const char *key, MODE default_value);
    long decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value);
    String decode_json_key_as_name(JsonDocument &doc, const char *key, const char *default_value);
};

#endif

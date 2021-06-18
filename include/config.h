#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include "sensor.h"
#include "actuator.h"

enum class MODE { SETUP = 0, NORMAL };

class Config {
public:
    Config();
    ~Config();
    void reset();
    void read();
    MODE get_mode();
    char *get_ntp_server();
    long int get_ntp_gmt_offset();
    long int get_ntp_daylight_offset();
    const char *get_wifi_ssid();
    const char *get_wifi_password();

private:
    MODE m_mode = MODE::SETUP;

    char *m_ntp_server = nullptr;

    long int m_ntp_gmt_offset = 0;
    long int m_ntp_daylight_offset = 0;

    char *m_wifi_ssid = nullptr;
    char *m_wifi_password = nullptr;

    std::vector<Sensor*> m_sensors;

    Relay* m_cold;
    Relay* m_heat;
    Relay* m_fan;
    Relay* m_light;

    MODE decode_json_key_as_mode(JsonDocument &doc, const char *key, MODE default_value);
    long decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value);
    char *decode_json_key_as_name(JsonDocument &doc, const char *key, const char *default_value);
};

#endif

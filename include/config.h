#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include "sensor.h"
#include "actuator.h"
#include "timetable.h"

#define CONFIG_FILENAME "/config.json"

enum class MODE { SETUP = 0, NORMAL };
enum class HANDLER { TEST, BASIC, PID };


class Config {
public:
    Config();
    ~Config();
    void reset();
    void read();

    MODE get_mode();
    HANDLER get_handler();
    String get_mode_string();
    String get_handler_string();
    String get_ntp_server();
    long int get_ntp_gmt_offset();
    long int get_ntp_daylight_offset();
    String get_wifi_ssid();
    String get_wifi_password();

    float get_temperature_reference();

    std::vector<Sensor*> sensors;
    std::vector<Timetable*> schedule;

    Sensor_DOOR *door = nullptr;
    Sensor_BUTTON *button = nullptr;

    Actuator *cold = nullptr;
    Actuator *heat = nullptr;
    Actuator *fan = nullptr;
    Actuator *light = nullptr;

private:
    MODE m_mode = MODE::SETUP;
    HANDLER m_handler = HANDLER::TEST;

    String m_ntp_server = "";

    long int m_ntp_gmt_offset = 0;
    long int m_ntp_daylight_offset = 0;

    String m_wifi_ssid = "";
    String m_wifi_password = "";

    MODE decode_json_key_as_mode(JsonDocument &doc, const char *key, MODE default_value);
    HANDLER decode_json_key_as_handler(JsonDocument& doc, const char* key, HANDLER default_value);
    long decode_json_key_as_long(JsonDocument &doc, const char *key, long int default_value);
    String decode_json_key_as_name(JsonDocument &doc, const char *key, const char *default_value);

    Actuator* set_actuator(String type, int value);
};

#endif

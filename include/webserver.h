#ifndef SB_WEBSERVER_H
#define SB_WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include "config.h"
#include "state.h"

enum class WIFI_MODE { OFF, ACCESS_POINT, CLIENT };
class WebServer {
public:
    TaskHandle_t task;
    WebServer(Config *config, State *state);
    void loop();


private:
    Config *m_config;
    State *m_state;

    bool m_initialized = false;
    AsyncWebServer *m_webserver = nullptr;
    WIFI_MODE m_wifi_mode = WIFI_MODE::OFF;

    void init();
    void set_default_time();
    void check_ntp();
    void init_access_point();
    void init_wifi_client();
    void init_server();
    void send_data();


};

String html_preprocessor(const String &var);

void save_setup(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
String GetBodyContent(uint8_t *data, size_t len);


#endif

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "config.h"
#include "state.h"

class WebServer {
public:
    TaskHandle_t task;
    WebServer(Config *config, State *state);
    void loop();

private:
    Config *m_config;
    State *m_state;
    bool m_initialized = false;

    void init();
    void set_default_time();
    void check_ntp();
    void init_access_point();
    void init_wifi_client();
    void init_server();
    void send_data();
};

#endif

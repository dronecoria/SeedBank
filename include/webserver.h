#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "config.h"
#include "state.h"

class WebServer {
public:
    TaskHandle_t task;
    WebServer(Config *config, State *state);

private:
    Config *m_config;
    State *m_state;
};

#endif

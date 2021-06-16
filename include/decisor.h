#ifndef DECISOR_H
#define DECISOR_H

#include "config.h"
#include "state.h"

class Decisor {
public:
    TaskHandle_t task;
    Decisor(Config *config, State *state);
    void loop();

private:
    Config *m_config;
    State *m_state;
};

#endif

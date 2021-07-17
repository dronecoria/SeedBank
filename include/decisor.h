#ifndef DECISOR_H
#define DECISOR_H

#include <functional>
#include "config.h"
#include "state.h"
#include "handler.h"

class Decisor {
public:
    TaskHandle_t task;
    Decisor(Config *config, State *state);
    void loop(float deltatime);

    void select_handler(HANDLER_TYPE handler);
private:
    Handler *m_handler = nullptr;

    Config *m_config;
    State *m_state;
};

#endif

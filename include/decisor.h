#ifndef DECISOR_H
#define DECISOR_H

#include <functional>
#include "config.h"
#include "state.h"

class Decisor {
public:
    TaskHandle_t task;
    Decisor(Config *config, State *state);
    void loop();

    void select_handler(HANDLER handler);
private:
    void set_handler(std::function<void()> handler);
    std::function<void()> m_handler;

    Config *m_config;
    State *m_state;
};

#endif

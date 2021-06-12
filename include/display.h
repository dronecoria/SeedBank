#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "state.h"

class Display {
public:
    TaskHandle_t task;
    Display(Config *config, State *state);

private:
    Config *m_config;
    State *m_state;
};

#endif

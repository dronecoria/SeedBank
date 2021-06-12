#ifndef STATE_H
#define STATE_H

#include "config.h"


class State {
public:
    State(Config *config);
    bool is_clock_set();
private:
    Config *m_config;
    bool m_is_clock_set = false;
};

#endif

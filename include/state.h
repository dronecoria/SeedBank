#ifndef STATE_H
#define STATE_H

#include "config.h"


class State {
public:
    State(Config *config);
    bool is_clock_set = false;

    float get_avg_temperature();
    void print_all_temperatures();

private:
    Config *m_config;
};

#endif

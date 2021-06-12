#include "state.h"

State::State(Config *config) {
    Serial.println("Setup State");
    this->m_config = config;
    // TODO
    this->m_is_clock_set = true;
}

bool State::is_clock_set() {
    return this->m_is_clock_set;
}

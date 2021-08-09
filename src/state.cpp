#include "state.h"
#include "sensor.h"

State::State(Config *config) {
    SERIAL_PRINTLN("Setup State");
    this->m_config = config;
    // TODO
}

float State::get_avg_temperature() {
    float temperatures = 0.0;
    int count = 0;
    for (Sensor *s : this->m_config->sensors) {
        if(s->is_temperature()){
            float t = s->get_value();
            if (t != TEMP_ERROR_READING) {
                temperatures += t;
                count += 1;
            }
        }
    }
    if (count > 0) {
        return temperatures / count;
    }
    return TEMP_ERROR_READING;
}

void State::sensors_update() {
    for (Sensor* s : this->m_config->sensors) {
        s->update();
    }
}

void State::print_all_temperatures() {
#ifdef DEBUG_SERIAL
    SERIAL_PRINT("[");
    for (Sensor *s : this->m_config->sensors) {
        if (s->is_temperature()) {
            SERIAL_PRINT(s->get_value());
            SERIAL_PRINT(", ");
        }
    }
    SERIAL_PRINT("]");
#endif
}

#include "state.h"
#include "sensor.h"

State::State(Config *config) {
    Serial.println("Setup State");
    this->m_config = config;
    // TODO
}

float State::get_avg_temperature() {
    float temperatures = 0.0;
    int count = 0;
    for (Sensor *s : this->m_config->sensors) {
        float t = s->get_last_value();
        if (t != TEMP_ERROR_READING) {
            temperatures += t;
            count += 1;
        }
    }
    if (count > 0) {
        return temperatures / count;
    }
    return TEMP_ERROR_READING;
}

void State::sensors_update() {
    for (Sensor* s : this->m_config->sensors) {
        if (s->get_value() == TEMP_ERROR_READING){
            Serial.println("ERROR reading sesnsor value");
        }
    }
}

void State::print_all_temperatures() {
    Serial.print("[");
    for (Sensor *s : this->m_config->sensors) {
        Serial.print(s->get_last_value());
        Serial.print(", ");
    }
    Serial.print("]");
}

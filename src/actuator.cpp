#include <Arduino.h>
#include "actuator.h"

Actuator::Actuator() {}

bool Actuator::is_active() {
    return m_is_active;
}

Relay::Relay(int pin) {
    this->m_pin = pin;
    this->m_type = ACTUATOR_TYPE::RELAY;
    pinMode(this->m_pin, OUTPUT);
    this->disable();
}

void Relay::enable() {
    digitalWrite(this->m_pin, LOW);
    this->m_is_active = true;
}

void Relay::disable() {
    digitalWrite(this->m_pin, HIGH);
    this->m_is_active = false;
}

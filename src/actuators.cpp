#include "actuators.h"

Actuator::Actuator() {}

bool Actuator::isActive() { return m_isActive; }

Relay::Relay(int _pin) {
  pin = _pin;
  type = ACTUATOR_TYPE::RELAY;
  pinMode(_pin, OUTPUT);
  disable();
}

void Relay::enable() {
  digitalWrite(pin, LOW);
  m_isActive = true;
}

void Relay::disable() {
  digitalWrite(pin, HIGH);
  m_isActive = false;
}
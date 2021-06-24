#include <Arduino.h>
#include "actuator.h"

Actuator::Actuator() {}

bool Actuator::is_active() {
    return m_is_active;
}

void Actuator::set_value(float value)
{
    if(value > 0.01f){
        this->enable();
    }
    this->disable();
}

/*
 *   RELAY
 */
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

/*
 *   SOLID
 */
Solid::Solid(int pin) {
    this->m_pin = pin;
    this->m_type = ACTUATOR_TYPE::SOLID;
    pinMode(this->m_pin, OUTPUT);
    this->disable();
}

void Solid::enable() {
    digitalWrite(this->m_pin, LOW);
    this->m_is_active = true;
}

void Solid::disable() {
    digitalWrite(this->m_pin, HIGH);
    this->m_is_active = false;
}

/*
 *   PWM
 */
int number_pwm_used = 0;

Pwm::Pwm(int pin) {
    this->m_pin = pin;
    this->m_type = ACTUATOR_TYPE::PWM;

    ledcAttachPin(pin, m_pwm_channel + number_pwm_used);
    ledcSetup(m_pwm_channel, m_pwm_freq, m_pwm_res);

    number_pwm_used++;
    this->disable();
}

void Pwm::set_value(float value)
{
    value = max(0.0f, min(1.0f, value));
    int steps = pow(2, m_pwm_res);
    float duty_cycle = value * steps-1;

    //8 bit resolution -> 256 steps (2^8)
    ledcWrite(m_pwm_channel, duty_cycle );
    m_value = value;
    this->m_is_active = (value > 0.01f);
}

float Pwm::get_value()
{
    return m_value;
}

void Pwm::enable() {
    this->set_value(1);
}

void Pwm::disable() {
    this->set_value(0);
}

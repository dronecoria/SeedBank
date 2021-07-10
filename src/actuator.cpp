#include "actuator.h"

Actuator::Actuator() {}

bool Actuator::is_active() {
    return m_is_active;
}

String Actuator::get_type() {
    switch (m_type)
    {
        case ACTUATOR_TYPE::RELAY:
            return "RELAY";
        case ACTUATOR_TYPE::SOLID:
            return "SOLID";
        case ACTUATOR_TYPE::PWM:
            return"PWM";

        default:
         return "unknow";
    }

}

float Actuator::get_value(){
    return static_cast<float>(this->is_active());
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
 *   PWM by software
 */

SoftPwm::SoftPwm(int pin) {
    this->m_pin = pin;
    this->m_type = ACTUATOR_TYPE::PWM;

    pinMode(pin, OUTPUT);
    m_pwmTickTime = 0;

    xTaskCreate(
        loop_softPWM_task,  // Task function.
        "softPWM",          // String with name of task.
        1000,              // Stack size in bytes.
        (void*)this,        // Parameter passed as input of the task
        1,                  // Priority of the task.
        &this->m_task);     // Task handle.

    this->disable();
}

void loop_softPWM_task(void* p_softPWM) {
    SoftPwm* softPwm = (SoftPwm*)p_softPWM;
    unsigned long currentMicros = micros();
    unsigned long previousMicros = 0;

    while (true) {
        if(softPwm->is_active()){
            currentMicros = millis();
            softPwm->handlePWM(currentMicros - previousMicros);
            previousMicros = currentMicros;
        }
        vTaskDelay((softPwm->frequency()) / portTICK_PERIOD_MS);
    }
}

void SoftPwm::handlePWM(unsigned long deltatime) {

    m_pwmTickTime += deltatime;

    float step = min(1.0f, ((float)m_pwmTickTime / (float)interval_time));  // between 0 and 1

    digitalWrite(m_pin, (step < m_value) );

    if (m_pwmTickTime > interval_time) {
        m_pwmTickTime -= interval_time;
    }
}

void SoftPwm::set_value(float value)
{
    value = max(0.0f, min(100.0f, value));

    m_value = value;
    m_is_active = (value > 0.001f);
}

float SoftPwm::get_value()
{
    return m_value;
}

void SoftPwm::enable() {
    m_value = 1;
    m_is_active = true;
}

void SoftPwm::disable() {
    m_value = 0;
    m_is_active = false;
}



/*
 *  REAL PWM
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

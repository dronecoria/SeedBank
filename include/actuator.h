#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>

enum class ACTUATOR_TYPE { NONE, RELAY, SOLID, PWM };

class Actuator {
public:
    Actuator();
    virtual void enable();
    virtual void disable();

    virtual void set_value(float value);
    virtual float get_value();

    String get_type();
    bool is_active();

protected:
    ACTUATOR_TYPE m_type = ACTUATOR_TYPE::NONE;
    int m_pin = -1;
    bool m_is_active = false;
};

class Relay : public Actuator {
public:
    Relay(int pin);

    void enable();
    void disable();
};

class Solid : public Actuator {
public:
    Solid(int pin);

    void enable();
    void disable();
};

class SoftPwm : public Actuator {
public:
    SoftPwm(int pin);

    void enable() override;
    void disable() override;
    void set_value(float value) override;
    float get_value() override;

    void handlePWM(unsigned long deltatime);
    inline int frequency(){return interval_time/steps;};
private:

    TaskHandle_t m_task;
   // float m_pwmValue;
    //bool  m_pwmState;
    int   m_pwmTickTime;

    float m_value = 0.0f;

    const int interval_time = 1000;
    const int steps = 100;
};
void loop_softPWM_task(void* p_softPWM);


class Pwm : public Actuator {
public:
    Pwm(int pin);

    void enable() override;
    void disable() override;
    void set_value(float value) override;
    float get_value() override;

private:
    float m_value = 0.0f;
    // https://deepbluembedded.com/esp32-pwm-tutorial-examples-analogwrite-arduino/
    int m_pwm_channel = 0;
    int m_pwm_res = 13;
    int m_pwm_freq = 5000;
};


#endif

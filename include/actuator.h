#ifndef ACTUATOR_H
#define ACTUATOR_H

enum class ACTUATOR_TYPE { NONE, RELAY, SOLID };

class Actuator {
public:
    Actuator();
    virtual void enable();
    virtual void disable();

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



#endif

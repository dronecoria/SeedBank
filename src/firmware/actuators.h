#ifndef ACTUATORS_H
#define ACTUATORS_H


enum class ACTUATOR_TYPE{
  NONE,RELAY,SOLID
};

class Actuator{  
public:
  ACTUATOR_TYPE type = ACTUATOR_TYPE::NONE;
  int pin = -1;

  Actuator(){};
  virtual void enable();
  virtual void disable();

  bool isActive(){ return m_isActive;};
protected:
  bool m_isActive = false;
};

class Relay: public Actuator
{
public:
  Relay(int _pin){
    this->pin = _pin;
    this->type = ACTUATOR_TYPE::RELAY;
    pinMode(_pin, OUTPUT);
    this->disable();
  };

  virtual void enable()
  {
    digitalWrite(pin, LOW);
    m_isActive = true;
  }
  
  virtual void disable()
  {
    digitalWrite(pin, HIGH);
    m_isActive = false;
  }
};


#endif
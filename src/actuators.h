#ifndef ACTUATORS_H
#define ACTUATORS_H
// for OUTPUT & HIGH LOW constants
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

enum class ACTUATOR_TYPE { NONE, RELAY, SOLID };

class Actuator {
public:
  ACTUATOR_TYPE type = ACTUATOR_TYPE::NONE;
  int pin = -1;

  Actuator();
  virtual void enable();
  virtual void disable();

  bool isActive();

protected:
  bool m_isActive = false;
};

class Relay : public Actuator {
public:
  Relay(int _pin);

  void enable();
  void disable();
};

#endif
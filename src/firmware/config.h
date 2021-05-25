#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include "sensors.h"
#include "actuators.h"

class Config{
public:
  //sensors
  std::vector<Sensor*> sensors;  
  
  //int pin_door= -1;

  //relays
  Relay *cold;
  Relay *hot;
  Relay *fan;
  Relay *light;

  Config(){};
  ~Config();

  void init(void);


private:
};


#endif

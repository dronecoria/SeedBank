#ifndef STATUS_H
#define STATUS_H

#include <vector>
#include "config.h"

enum class DISPLAY_STATUS
{
  NORMAL = 0, INFO, OFF
};

class Status{
public:
  //sensors
  std::vector<float> temperatures;
  bool door;

  //relays
  // bool cold = false;
  // bool hot = false;
  // bool fan = false;
  // bool light = false;

  //calculated
  float avg_temperature;

  Status(){};

  void init(Config *config);

  void update(void);
  //void setRelay(ACTION relay,bool active);

  Config* config(){return m_config;}; 

  DISPLAY_STATUS getDisplayStatus(){ return m_display_status;};
  void nextDisplayStatus(){
    if(m_display_status == DISPLAY_STATUS::OFF){
      m_display_status = (DISPLAY_STATUS)0;
    }else{
      m_display_status = (DISPLAY_STATUS)( ((int)m_display_status) + 1);
    }
  }

private:
  //std::vector<Sensor*> m_sensors;  //TODO class sensor
  Config *m_config;
  DISPLAY_STATUS m_display_status;
};


#endif
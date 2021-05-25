#include "status.h"


void Status::init(Config *config){
  m_config = config;
}

void Status::update(void){
    this->temperatures.clear();
    for(auto sensor : m_config->sensors){
      float t = sensor->getValue();
      this->temperatures.push_back(t);
    }
    if(this->temperatures.size() >= 2){
      this->avg_temperature = 0;
      for(auto t : this->temperatures){
        this->avg_temperature += t;    
      }  
      this->avg_temperature /= this->temperatures.size();
    }else{
      this->avg_temperature = -9999;
    }

    if(this->temperatures[0]>25){
      //setRelay(ACTION::COLD,true);
      m_config->cold->enable();
    }else{
      //setRelay(ACTION::COLD,false);
      m_config->cold->disable();
    }
    if(this->temperatures[1]>25){
      //setRelay(ACTION::LIGHT,true);
      m_config->light->enable();
    }else{
      //setRelay(ACTION::LIGHT,false);
      m_config->light->disable();
    }
  }

  // void Status::setRelay(ACTION relay,bool active){
  //   switch(relay){
  //     case ACTION::COLD:
  //       this->cold = active;
  //       break;
  //     case ACTION::HOT:
  //       this->hot = active;
  //       break;
  //     case ACTION::FAN:
  //       this->fan = active;
  //       break;
  //     case ACTION::LIGHT:
  //       this->light = active;
  //       break;
  //   }
  // }

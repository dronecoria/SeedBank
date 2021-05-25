#include "config.h"

void Config::init(void){

  this->sensors.push_back( new Sensor_BMP280(0x76) );
  //this->sensors.push_back( new Sensor_DUMMY(0) );
  this->sensors.push_back( new Sensor_BMP280(0x77) );
  
  this->cold   = new Relay(0);
  this->hot    = new Relay(15);
  this->fan    = new Relay(16);
  this->light  = new Relay(17);
/*
   for(auto sensor : json_read_sensors)
    {
      switch(sensor.type){
        case SENSOR_TYPE::DHT22:
          break;
        case SENSOR_TYPE::BMP280:
          m_sensors.push_back( new Sensor_BMP280(sensor.pin) );
          break;
        case SENSOR_TYPE::DUMMY:
          m_sensors.push_back( new Sensor_DUMMY(sensor.pin) );
          break;
        case SENSOR_TYPE::TEST:
          break;
      }
    }
*/

}

Config::~Config(){
  Serial.println(F("destroy sensors"));
  for(auto sensor : this->sensors){
    delete sensor;
  }
  Serial.println(F("destroyed sensors"));
}
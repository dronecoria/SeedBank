#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_BMP280.h>

enum class SENSOR_TYPE{
  DHT22,BMP280,
  DUMMY,TEST,
  DOOR,BUTTON
};

class Sensor{
public:
   SENSOR_TYPE type;
   int pin = -1;  // pin or adress for i2c

   Sensor(){};
   virtual float getValue(void);
};



class Sensor_BMP280 : public Sensor{
public:  
  Sensor_BMP280(int _pin){
    this->type = SENSOR_TYPE::BMP280;
    this->pin = _pin;
    if (!m_sensor.begin(_pin)) {
        while (10){
          Serial.println(F("Could not find a valid BMP280 sensor check wiring!"));
        };
    }
    m_sensor.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  };
  float getValue(void){
    return m_sensor.readTemperature();
  };
private:
  Adafruit_BMP280 m_sensor;
};

class Sensor_DUMMY : public Sensor{
public:
  Sensor_DUMMY(int _pin){
    this->type = SENSOR_TYPE::BMP280;
    this->pin = _pin;
  };
  float getValue(void){
    return 42;
  };
};


#endif

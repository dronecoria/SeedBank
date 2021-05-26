#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_BMP280.h>

enum class SENSOR_TYPE { DHT22, BMP280, DUMMY, TEST, DOOR, BUTTON };

class Sensor {
public:
  SENSOR_TYPE type;
  int pin = -1; // pin or adress for i2c

  Sensor();
  virtual float getValue(void);
};

class Sensor_BMP280 : public Sensor {
public:
  Sensor_BMP280(int _pin);
  float getValue(void);

private:
  Adafruit_BMP280 m_sensor;
};

class Sensor_DUMMY : public Sensor {
public:
  Sensor_DUMMY(int _pin);
  float getValue(void);
};

#endif

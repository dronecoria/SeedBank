#include "sensors.h"

Sensor::Sensor() {}

/**
 * Sensor_DUMMY
 **/
Sensor_DUMMY::Sensor_DUMMY(int _pin) {
  this->type = SENSOR_TYPE::DUMMY;
  this->pin = _pin;
}

float Sensor_DUMMY::getValue(void) { return 42; }

/**
 * Sensor_BMP280
 **/
Sensor_BMP280::Sensor_BMP280(int _pin) {
  this->type = SENSOR_TYPE::BMP280;
  this->pin = _pin;
  if (!m_sensor.begin(_pin)) {
    while (10) {
      Serial.println(F("Could not find a valid BMP280 sensor check wiring!"));
      Serial.println(_pin);
    };
  }
  m_sensor.setSampling(
    Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

float Sensor_BMP280::getValue(void) { return m_sensor.readTemperature(); }

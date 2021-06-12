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

/**
 * Sensor_HDC2080
 **/
Sensor_HDC2080::Sensor_HDC2080(int _pin) {
    this->type = SENSOR_TYPE::HDC2080;
    this->pin = _pin;
    this->m_sensor = new HDC2080(this->pin);
    Serial.print(" - Configure HDC2080 sensor ... ");
    this->m_sensor->begin();
    this->m_sensor->reset();
    this->m_sensor->setHighTemp(48);
    this->m_sensor->setLowTemp(2);
    this->m_sensor->setHighHumidity(95);
    this->m_sensor->setLowHumidity(10);
    this->m_sensor->setMeasurementMode(TEMP_AND_HUMID);
    this->m_sensor->setRate(ONE_HZ);
    this->m_sensor->setTempRes(FOURTEEN_BIT);
    this->m_sensor->setHumidRes(FOURTEEN_BIT);
    this->m_sensor->triggerMeasurement();
    Serial.println("OK");

}

float Sensor_HDC2080::getValue(void) {
    float temp;
    temp = this->m_sensor->readTemp();
    Serial.print("Temp: ");
    Serial.println(temp);
    return temp;
}

#include "sensor.h"


Sensor::Sensor() {}

// --------------------------------------------------------------------------
// Sensor HDC2080: Texas Instruments, i2C interfaz (0x40, 0x41), +/- 0.2 ºC
// --------------------------------------------------------------------------
Sensor_HDC2080::Sensor_HDC2080(int address) {
    this->m_type = SENSOR_TYPE::HDC2080;
    this->m_pin = address;
    this->m_sensor = new HDC2080(this->m_pin);
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
    // if (!this) {
    //     Serial.print(F("Could not find a BMP280 sensor at address "));
    //     Serial.println(address);
    // }
}

Sensor_HDC2080::~Sensor_HDC2080() {
    if (this->m_sensor != nullptr) {
        delete this->m_sensor;
        this->m_sensor = nullptr;
    }
}

float Sensor_HDC2080::get_value() {
    if (this->m_sensor != nullptr) {
        return this->m_sensor->readTemp();
    }
    return 0.0;
}


// --------------------------------------------------------------------------
// Sensor BMP280: Bosch, i2C interfaz (0x76, 0x77), +/- 0.5 ºC
// --------------------------------------------------------------------------
Sensor_BMP280::Sensor_BMP280(int address) {
    // TODO
}

Sensor_BMP280::~Sensor_BMP280() {
    // TODO
}

float Sensor_BMP280::get_value() {
    // TODO
    return 0.0;
}


// --------------------------------------------------------------------------
// Sensor DOOR: Generic input state sensor
// --------------------------------------------------------------------------
Sensor_DOOR::Sensor_DOOR(int pin) {
    // TODO
}

float Sensor_DOOR::get_value() {
    // TODO
    return 0.0;
}


// --------------------------------------------------------------------------
// Sensor BUTTON: Generic button sensor with interrupt callback
// --------------------------------------------------------------------------
Sensor_BUTTON::Sensor_BUTTON(int pin, void (*callback)(Sensor_BUTTON *button)) {
    // TODO
}

float Sensor_BUTTON::get_value() {
    // TODO
    return 0.0;
}

// --------------------------------------------------------------------------
// Sensor DUMMY: Testing proposes, returns random temp values between a range with a maximum variation
// --------------------------------------------------------------------------
Sensor_DUMMY::Sensor_DUMMY(float min, float max, float max_variation) {
    // TODO
}

float Sensor_DUMMY::get_value() {
    // TODO
    return 0.0;
}

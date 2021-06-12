#include "sensor.h"


Sensor::Sensor() {}

// --------------------------------------------------------------------------
// Sensor HDC2080: Texas Instruments, i2C interfaz (0x40, 0x41), +/- 0.2 ºC
// --------------------------------------------------------------------------
Sensor_HDC2080::Sensor_HDC2080(int address) {
    // TODO
}

Sensor_HDC2080::~Sensor_HDC2080() {
    // TODO
}

float Sensor_HDC2080::get_value() {
    // TODO
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

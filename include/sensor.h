#ifndef SENSOR_H
#define SENSOR_H

#include <HDC2080.h>
#include <Adafruit_BMP280.h>

enum class SENSOR_TYPE { NONE, DHT22, BMP280, HDC2080, DUMMY, DOOR, BUTTON };

// --------------------------------------------------------------------------
// Sensor base class
// --------------------------------------------------------------------------
class Sensor {
public:
    Sensor();
    virtual float get_value();

protected:
    SENSOR_TYPE m_type = SENSOR_TYPE::NONE;
    int m_pin = -1;
};

// --------------------------------------------------------------------------
// Sensor HDC2080: Texas Instruments, i2C interfaz (0x40, 0x41), +/- 0.2 ºC
// --------------------------------------------------------------------------
class Sensor_HDC2080 : public Sensor {
public:
    Sensor_HDC2080(int address);
    ~Sensor_HDC2080();
    float get_value(void);

private:
    HDC2080 *m_sensor = nullptr;
};

// --------------------------------------------------------------------------
// Sensor BMP280: Bosch, i2C interfaz (0x76, 0x77), +/- 0.5 ºC
// --------------------------------------------------------------------------
class Sensor_BMP280 : public Sensor {
public:
    Sensor_BMP280(int address);
    ~Sensor_BMP280();
    float get_value(void);

private:
    Adafruit_BMP280 *m_sensor = nullptr;
};

// --------------------------------------------------------------------------
// Sensor DOOR: Generic input state sensor
// --------------------------------------------------------------------------
class Sensor_DOOR : public Sensor {
public:
    Sensor_DOOR(int pin);
    float get_value(void);
};

// --------------------------------------------------------------------------
// Sensor BUTTON: Generic button sensor with interrupt callback
// --------------------------------------------------------------------------
class Sensor_BUTTON : public Sensor {
public:
    Sensor_BUTTON(int pin, void (*callback)(Sensor_BUTTON *button));
    float get_value(void);

private:
    unsigned long m_last_interrupt_time = 0;
    void (*callback)(Sensor_BUTTON *button) = nullptr;
};

// --------------------------------------------------------------------------
// Sensor DUMMY: Testing proposes, returns random temp values between a range with a maximum variation
// --------------------------------------------------------------------------
class Sensor_DUMMY : public Sensor {
public:
    Sensor_DUMMY(float min, float max, float max_variation);
    float get_value(void);

private:
    float m_min;
    float m_max;
    float m_max_variation;
    float m_current;
};



#endif

#ifndef SENSOR_H
#define SENSOR_H

#include <Wire.h>
#include <HDC2080.h>
#include <Adafruit_BMP280.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "defines.h"



#ifndef DS18B20_TEMP_PRECISION
#define DS18B20_TEMP_PRECISION 12
#endif

#ifndef DS18B20_MAX_COUNT
#define DS18B20_MAX_COUNT 2
#endif

enum class SENSOR_TYPE { NONE, DHT22, DS18B20, BMP280, HDC2080, DUMMY, DOOR, BUTTON };

// --------------------------------------------------------------------------
// Sensor base class
// --------------------------------------------------------------------------
class Sensor {
public:
    Sensor();
    virtual void update();
    virtual void reload();

    float get_value();
    String get_type_string();
    bool is_temperature();
    inline SENSOR_TYPE get_type() {return m_type;}
    inline int get_pin(){return m_pin;}

protected:
    SENSOR_TYPE m_type = SENSOR_TYPE::NONE;
    int m_pin = -1;
    float m_last_value = TEMP_ERROR_READING;
};

// --------------------------------------------------------------------------
// Sensor DS18B20: Maxim, 1-Wire interfaz, +/- 0.5 ºC
// --------------------------------------------------------------------------
class Sensor_DS18B20 : public Sensor {
public:
    Sensor_DS18B20(int pin);
    ~Sensor_DS18B20();
    void update() override;
    void reload() override {};

    int get_count(void);
    float get_value_by_index(int index);

private:
    OneWire *m_bus = nullptr;
    DallasTemperature *m_sensors = nullptr;
    DeviceAddress m_addresses[DS18B20_MAX_COUNT];
    int m_count = 0;

    float m_get_value_by_index(int index);
};

// --------------------------------------------------------------------------
// Sensor HDC2080: Texas Instruments, i2C interfaz (0x40, 0x41), +/- 0.2 ºC
// --------------------------------------------------------------------------
class Sensor_HDC2080 : public Sensor {
public:
    Sensor_HDC2080(int address);
    ~Sensor_HDC2080();
    void update() override;
    void reload() override;

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
    void update() override;
    void reload() override {};

private:
    Adafruit_BMP280 *m_sensor = nullptr;
};

// --------------------------------------------------------------------------
// Sensor DOOR: Generic input state sensor
// --------------------------------------------------------------------------
class Sensor_DOOR : public Sensor {
public:
    Sensor_DOOR(int pin);
    void update() override;
    void reload() override {};
};

// --------------------------------------------------------------------------
// Sensor BUTTON: Generic button sensor with interrupt callback
// --------------------------------------------------------------------------
class Sensor_BUTTON : public Sensor {
public:
    Sensor_BUTTON(int pin, void (*callback)(Sensor_BUTTON *button));
    void update() override;
    void reload() override {};

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
    void update() override;
    void reload() override {};

private:
    float m_min;
    float m_max;
    float m_max_variation;
    float m_current;
};



#endif

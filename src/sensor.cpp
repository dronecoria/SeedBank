#include "sensor.h"


Sensor::Sensor() {}
float Sensor::get_last_value() { return this->m_last_value; }

// --------------------------------------------------------------------------
// Sensor DS18B20: Maxim, 1-Wire interfaz, +/- 0.5 ºC
// --------------------------------------------------------------------------
Sensor_DS18B20::Sensor_DS18B20(int pin) {
    int i = 0;

    this->m_type = SENSOR_TYPE::DS18D20;
    this->m_pin = pin;
    this->m_bus = new OneWire(this->m_pin);
    this->m_sensors = new DallasTemperature(this->m_bus);
    this->m_sensors->begin();
    while ((i < DS18B20_MAX_COUNT) && (this->m_sensors->getAddress(this->m_addresses[i], i) == true)) {
        this->m_sensors->setResolution(this->m_addresses[i], DS18B20_TEMP_PRECISION);
        i++;
    }
    this->m_count = i;
}

Sensor_DS18B20::~Sensor_DS18B20() {
    if (this->m_bus != nullptr) {
        delete this->m_bus;
        this->m_bus = nullptr;
    }
    if (this->m_sensors != nullptr) {
        delete this->m_sensors;
        this->m_sensors = nullptr;
    }
}

int Sensor_DS18B20::get_count(void) {
    return this->m_count;
}

float Sensor_DS18B20::get_value_by_index(int index) {
    this->m_sensors->requestTemperaturesByAddress(this->m_addresses[index]);
    return this->m_get_value_by_index(index);
}

float Sensor_DS18B20::m_get_value_by_index(int index) {
    return this->m_sensors->getTempC(this->m_addresses[index]);
}

float Sensor_DS18B20::get_value(void) {
    int count = this->m_count;
    float temperatures = 0.0;

    this->m_sensors->requestTemperatures();
    for (int i = 0; i < count; i++) {
        temperatures += this->m_get_value_by_index(i);
    }
    if (count > 0) {
        this->m_last_value = temperatures / count;
        return this->m_last_value;
    }
    return TEMP_ERROR_READING;
}

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
}

Sensor_HDC2080::~Sensor_HDC2080() {
    if (this->m_sensor != nullptr) {
        delete this->m_sensor;
        this->m_sensor = nullptr;
    }
}

float Sensor_HDC2080::get_value() {
    if (this->m_sensor != nullptr) {
        float t = this->m_sensor->readTemp();
        if (t == -40.0) {
            return TEMP_ERROR_READING;
        }
        this->m_last_value = t;
        return t;
    }
    return TEMP_ERROR_READING;
}


// --------------------------------------------------------------------------
// Sensor BMP280: Bosch, i2C interfaz (0x76, 0x77), +/- 0.5 ºC
// --------------------------------------------------------------------------
Sensor_BMP280::Sensor_BMP280(int address) {
    this->m_type = SENSOR_TYPE::BMP280;
    this->m_pin = address;
    this->m_sensor = new Adafruit_BMP280();
    if (!this->m_sensor->begin(this->m_pin)) {
        Serial.print("Could not find a BMP280 sensor at address ");
        Serial.println(this->m_pin);
    }
    this->m_sensor->setSampling(
        Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
        Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
        Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
        Adafruit_BMP280::FILTER_X16,      // Filtering.
        Adafruit_BMP280::STANDBY_MS_500   // Standby time.
    );
}

Sensor_BMP280::~Sensor_BMP280() {
    if (this->m_sensor != nullptr) {
        delete this->m_sensor;
        this->m_sensor = nullptr;
    }
}

float Sensor_BMP280::get_value() {
    if (this->m_sensor != nullptr) {
        this->m_last_value = this->m_sensor->readTemperature();
        return this->m_last_value;
    }
    return TEMP_ERROR_READING;
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

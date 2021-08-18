#include <tuple>

#include "sensor.h"

Sensor::Sensor() {}
float Sensor::get_value() { return this->m_last_value; }

bool Sensor::is_temperature()
{
    switch (m_type)
    {
    case SENSOR_TYPE::DHT22:
    case SENSOR_TYPE::DS18B20:
    case SENSOR_TYPE::BMP280:
    case SENSOR_TYPE::HDC2080:
        return true;
    case SENSOR_TYPE::DUMMY:
    case SENSOR_TYPE::DOOR:
    case SENSOR_TYPE::BUTTON:
    default:
        return false;
    }
}

String Sensor::get_type_string(){
    switch (m_type)
    {
        case SENSOR_TYPE::DHT22:
            return "DHT22";
        case SENSOR_TYPE::DS18B20:
            return "DS18B20";
        case SENSOR_TYPE::BMP280:
            return "BMP280";
        case SENSOR_TYPE::HDC2080:
            return "HDC2080";
        case SENSOR_TYPE::DUMMY:
            return "DUMMY";
        case SENSOR_TYPE::DOOR:
            return "DOOR";
        case SENSOR_TYPE::BUTTON:
            return "BUTTON";

        default:
            return "unknow";
    }

}
// --------------------------------------------------------------------------
// Sensor DS18B20: Maxim, 1-Wire interfaz, +/- 0.5 ºC
// --------------------------------------------------------------------------
Sensor_DS18B20::Sensor_DS18B20(int pin) {
    int i = 0;

    this->m_type = SENSOR_TYPE::DS18B20;
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

void Sensor_DS18B20::update(void) {
    int count = this->m_count;
    float temperatures = 0.0;

    this->m_sensors->requestTemperatures();
    for (int i = 0; i < count; i++) {
        temperatures += this->m_get_value_by_index(i);
    }
    if (count > 0) {
        this->m_last_value = temperatures / count;
    }
    this->m_last_value = TEMP_ERROR_READING;
}

// --------------------------------------------------------------------------
// Sensor HDC2080: Texas Instruments, i2C interfaz (0x40, 0x41), +/- 0.2 ºC
// --------------------------------------------------------------------------
Sensor_HDC2080::Sensor_HDC2080(int address) {
    this->m_type = SENSOR_TYPE::HDC2080;
    this->m_pin = address;
    reload();
}

Sensor_HDC2080::~Sensor_HDC2080() {
    if (this->m_sensor != nullptr) {
        delete this->m_sensor;
        this->m_sensor = nullptr;
    }
}

void Sensor_HDC2080::reload()
 {
    if(m_sensor != nullptr){
        delete m_sensor;
    }
    m_sensor = new HDC2080(this->m_pin);
    m_sensor->begin();
    m_sensor->reset();
    m_sensor->setHighTemp(48);
    m_sensor->setLowTemp(2);
    m_sensor->setHighHumidity(95);
    m_sensor->setLowHumidity(10);
    m_sensor->setMeasurementMode(TEMP_AND_HUMID);
    m_sensor->setRate(ONE_HZ);
    m_sensor->setTempRes(FOURTEEN_BIT);
    m_sensor->setHumidRes(FOURTEEN_BIT);
    m_sensor->triggerMeasurement();
 }

void Sensor_HDC2080::update() {
    float t = this->m_sensor->readTemp();
    if (t == -40.0) {
        t = TEMP_ERROR_READING;
        reload();
    }
    this->m_last_value = t;

}


// --------------------------------------------------------------------------
// Sensor BMP280: Bosch, i2C interfaz (0x76, 0x77), +/- 0.5 ºC
// --------------------------------------------------------------------------
Sensor_BMP280::Sensor_BMP280(int address) {
    this->m_type = SENSOR_TYPE::BMP280;
    this->m_pin = address;
    this->m_sensor = new Adafruit_BMP280();
    if (!this->m_sensor->begin(this->m_pin)) {
        SERIAL_PRINT("Could not find a BMP280 sensor at address ");
        SERIAL_PRINTLN(this->m_pin);
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

void Sensor_BMP280::update() {
    if (this->m_sensor == nullptr) {
        this->m_last_value = TEMP_ERROR_READING;
    }
    this->m_last_value = this->m_sensor->readTemperature();
}

// --------------------------------------------------------------------------
// Sensor DOOR: Generic input state sensor
// --------------------------------------------------------------------------
Sensor_DOOR::Sensor_DOOR(int pin) {
    m_pin = pin;
    m_type = SENSOR_TYPE::DOOR;

/*
Not work well, sometimes crash the program
    auto callback =  [](void* data) {
        Sensor_DOOR *sensor = static_cast<Sensor_DOOR*>(data);

        static unsigned long last_interrupt_time = 0;
        unsigned long interrupt_time = millis();
        if (interrupt_time - last_interrupt_time > 20) {
            int value = digitalRead(sensor->m_pin);
            if (value == 0) {
                SERIAL_PRINTLN(" DOOR OPENED");
                //TODO send info to logger
            }else{
                SERIAL_PRINTLN(" DOOR CLOSED");
                //TODO send info to logger
            }
        }
        last_interrupt_time = interrupt_time;
    };

    pinMode(m_pin, INPUT);
    attachInterruptArg(m_pin, callback, static_cast<void*>(this), CHANGE);
*/
    update();
}

void Sensor_DOOR::update() {
    m_last_value = static_cast<float>(digitalRead(m_pin));
}


// --------------------------------------------------------------------------
// Sensor BUTTON: Generic button sensor with interrupt callback
// --------------------------------------------------------------------------
Sensor_BUTTON::Sensor_BUTTON(int pin, void (*callback)(Sensor_BUTTON *button)) {
    // TODO
}

void Sensor_BUTTON::update() {
    // TODO
}

// --------------------------------------------------------------------------
// Sensor DUMMY: Testing proposes, returns random temp values between a range with a maximum variation
// --------------------------------------------------------------------------
Sensor_DUMMY::Sensor_DUMMY(float min, float max, float max_variation) {
    // TODO
}

void Sensor_DUMMY::update() {
    // TODO
}

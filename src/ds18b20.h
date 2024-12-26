#pragma once

#ifdef IOTCLOUD_ENABLE_DS18B20

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20
{
    enum Steps
    {
        GET_ADDR,
        REQ_TEMP,
        GET_TEMP
    };

public:
    float get_value();
    DS18B20(const int one_wire_bus);

private:
    DallasTemperature *_sensors;
    DeviceAddress _sensor_addr;
    OneWire *_one_wire;
    Steps _current_step = Steps::GET_ADDR;
};

#endif // IOTCLOUD_ENABLE_DS18B20
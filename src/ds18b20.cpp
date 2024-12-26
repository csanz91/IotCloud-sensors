#ifdef IOTCLOUD_ENABLE_DS18B20

#include "ds18b20.h"

DS18B20::DS18B20(const int one_wire_bus)
{
    _one_wire = new OneWire(one_wire_bus);

    _sensors = new DallasTemperature(_one_wire);

    _sensors->begin();
    _sensors->setWaitForConversion(false);
}

float DS18B20::get_value()
{
    float value = NULL;
    switch (_current_step)
    {
    case Steps::GET_ADDR:
    {
        Serial.println(F("Searching devices..."));
        bool found = _sensors->getAddress(_sensor_addr, 0);
        if (found)
        {
            Serial.print(F("Device found"));
            _sensors->setResolution(_sensor_addr, 12);
            _current_step = Steps::REQ_TEMP;
        }
        else
        {
            break;
        }
    }
    case Steps::REQ_TEMP:
    {
        _sensors->requestTemperaturesByAddress(_sensor_addr);
        _current_step = Steps::GET_TEMP;
    }
    break;
    case Steps::GET_TEMP:
    {
        float temperature = _sensors->getTempC(_sensor_addr);
        if (temperature == DEVICE_DISCONNECTED_C)
        {
            Serial.println(F("Error: Could not read temperature data"));
            break;
        }
        Serial.println(temperature);
        value = temperature;
        _current_step = Steps::REQ_TEMP;
    }
    break;
    }
    return value;
}

#endif // IOTCLOUD_ENABLE_DS18B20
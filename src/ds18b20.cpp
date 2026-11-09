#ifdef IOTCLOUD_ENABLE_DS18B20

#include "ds18b20.h"

static void print_address(const DeviceAddress address)
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        if (address[i] < 16)
        {
            Serial.print('0');
        }
        Serial.print(address[i], HEX);
    }
}

static bool raw_search_address(OneWire *one_wire, DeviceAddress address)
{
    one_wire->reset_search();
    bool found = one_wire->search(address);
    one_wire->reset_search();
    return found;
}

DS18B20::DS18B20(const int one_wire_bus)
{
    
    _one_wire = new OneWire(one_wire_bus);
    
    _sensors = new DallasTemperature(_one_wire);
    pinMode(one_wire_bus, INPUT_PULLUP);
    delay(5);
    
    _sensors->begin();
    _sensors->setWaitForConversion(false);
}

float DS18B20::get_value()
{
    float value = -273.0;
    switch (_current_step)
    {
    case Steps::GET_ADDR:
    {
        Serial.println(F("Searching devices..."));
        _sensor_found = _sensors->getAddress(_sensor_addr, 0);
        if (!_sensor_found)
        {
            _sensor_found = raw_search_address(_one_wire, _sensor_addr);
        }
        if (_sensor_found)
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

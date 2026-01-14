#include "thermostat_subsensor.h"
#include <ArduinoJson.h>

ThermostatSubsensor::ThermostatSubsensor(
    const char *sensor_id,
    const char *sensor_name,
    const int sensor_pin,
    const SI7021_AM2301::SensorModel sensor_model,
    const unsigned int report_value_period,
    const float filter_rate_change) : AnalogSensor(sensor_id,
                                                    sensor_name,
                                                    report_value_period,
                                                    1,  // 1 decimal for temperature
                                                    true,  // enable filter
                                                    "thermostat_subsensor",
                                                    "{\"engUnits\": \"\xB0" "C\", \"stringFormat\": \"#.0\"}",
                                                    filter_rate_change)
{
    _sensor = new SI7021_AM2301(sensor_pin, sensor_model);
    _custom_text[0] = '\0';
    _custom_text_expiration = 0;
}

void ThermostatSubsensor::init(char *mqtt_header, EspMQTTClient *mqtt_client)
{
    AnalogSensor::init(mqtt_header, mqtt_client);
    // Cache only the fields needed for topic construction (saves ~530 bytes vs full device_data)
    device_data data = get_data();
    strncpy(_cached_locationId, data.locationId, sizeof(_cached_locationId));
    strncpy(_cached_subdeviceId, data.subdeviceId, sizeof(_cached_subdeviceId));
    strncpy(_cached_subsensorId, data.subsensorId, sizeof(_cached_subsensorId));

    // Force an initial reading as soon as we are ready (MQTT is connected here)
    get_value();
}

void ThermostatSubsensor::construct_subsensor_topic(char *newTopic, size_t size, const char *endpoint)
{
    // If subdeviceId and subsensorId are provided in the config, use them for full impersonation
    if (_cached_subdeviceId[0] != '\0' && _cached_subsensorId[0] != '\0')
    {
        snprintf(newTopic, size, "v1/%s/%s/%s/%s", _cached_locationId, _cached_subdeviceId, _cached_subsensorId, endpoint);
    }
    else
    {
        // No impersonation configured, use own topic
        construct_topic(newTopic, size, endpoint);
    }
}

void ThermostatSubsensor::get_value()
{
    _sensor->read_values();
    
    float raw_temp = _sensor->temperature;
    float raw_humidity = _sensor->humidity;
    
    // Get humidity (no filtering needed)
    if (!isnan(raw_humidity) && !isinf(raw_humidity))
    {
        _humidity = raw_humidity;
    }
    
    // Use base class set_value() which handles filtering, offset, and calls report_value()
    // But we override report_value() to also publish humidity
    set_value(raw_temp);
    
    // Call display callback if registered
    if (_display_callback != nullptr)
    {
        _display_callback(value, _humidity);
    }
}

bool ThermostatSubsensor::report_value()
{
    // Make sure the client has been initialized
    if (!_mqtt_client || !_mqtt_client->isConnected())
        return false;

    // Publish temperature to subsensor topic (main thermostat's topic)
    if (!isnan(value))
    {
        char constructedTopic[160] = "";
        construct_subsensor_topic(constructedTopic, sizeof(constructedTopic), "value");
        char array[12];
        dtostrf(value, 1, _decimals, array);
        _mqtt_client->publish(constructedTopic, array, true);
    }
    
    // Publish humidity to subsensor topic
    if (!isnan(_humidity))
    {
        char constructedTopic[160] = "";
        construct_subsensor_topic(constructedTopic, sizeof(constructedTopic), "aux/humidity");
        char array[12];
        dtostrf(_humidity, 1, 1, array);  // 1 decimal for humidity
        _mqtt_client->publish(constructedTopic, array, true);
    }
    
    return true;
}

void ThermostatSubsensor::set_display_callback(DisplayCallback callback)
{
    _display_callback = callback;
}

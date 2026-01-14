#pragma once

#include <Arduino.h>
#include <sensors/analog.h>
#include "si7021_am2301.h"
#include <device_data.h>

// Callback function type for display updates
typedef void (*DisplayCallback)(float temperature, float humidity);

class ThermostatSubsensor : public AnalogSensor
{
private:
    SI7021_AM2301 *_sensor;
    float _humidity = NAN;
    
    // Display callback
    DisplayCallback _display_callback = nullptr;
    
    // Cached fields for topic construction (only what we need, not the full device_data)
    char _cached_locationId[LOCATIONID_MAX_SIZE];
    char _cached_subdeviceId[SUBDEVICEID_MAX_SIZE];
    char _cached_subsensorId[SUBSENSORID_MAX_SIZE];
    
    // Custom text display with TTL
    char _custom_text[32];
    unsigned long _custom_text_expiration;
    
    void get_value() override;
    bool report_value() override;
    void construct_subsensor_topic(char *newTopic, size_t size, const char *endpoint);

public:
    ThermostatSubsensor(
        const char *sensor_id,
        const char *sensor_name,
        const int sensor_pin,
        const SI7021_AM2301::SensorModel sensor_model = SI7021_AM2301::SensorModel::SI7021,
        const unsigned int report_value_period = 30000,  // milliseconds
        const float filter_rate_change = 0.1);  // 10% rate change for temperature (reject spikes like 17.5 -> 19.0)
    
    void init(char *mqtt_header, EspMQTTClient *mqtt_client) override;
    void set_display_callback(DisplayCallback callback);
    
    // Getters for display
    float get_temperature() const { return value; }
    float get_humidity() const { return _humidity; }
    
    // Custom text methods
    const char* get_custom_text() const { return _custom_text; }
    bool has_active_custom_text() const { return _custom_text[0] != '\0' && millis() < _custom_text_expiration; }
};

#pragma once

#ifdef IOTCLOUD_ENABLE_THERMOSTAT

#include <Arduino.h>
#include <sensors/analog.h>
#include "si7021_am2301.h"
#include <device_data.h>

// Callback function type for display updates
typedef void (*DisplayCallback)(float temperature, float humidity);
typedef void (*DisplayBrightnessCallback)(float brightness);

class ThermostatSubsensor : public AnalogSensor
{
private:
    SI7021_AM2301 *_sensor;
    float _humidity = NAN;
    
    // Display callback
    DisplayCallback _display_callback = nullptr;
    DisplayBrightnessCallback _display_brightness_callback = nullptr;
    
    // Cached fields for topic construction (only what we need, not the full device_data)
    char _cached_locationId[LOCATIONID_MAX_SIZE];
    char _cached_subdeviceId[SUBDEVICEID_MAX_SIZE];
    char _cached_subsensorId[SUBSENSORID_MAX_SIZE];
    
    // Custom text display with TTL
    char _custom_text[32];
    unsigned long _custom_text_expiration;

    // Display brightness (0.0 - 1.0)
    float _display_brightness = 1.0f;
    
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
        const float filter_rate_change = 0.017);  // ~0.3°C tolerance at 17-20°C
    
    void init(char *mqtt_header, EspMQTTClient *mqtt_client) override;
    void set_display_callback(DisplayCallback callback);
    void set_display_brightness_callback(DisplayBrightnessCallback callback);

    void set_display_brightness(float brightness, bool publish = true);
    float get_display_brightness() const { return _display_brightness; }
    
    // Getters for display
    float get_temperature() const { return value; }
    float get_humidity() const { return _humidity; }
    
    // Custom text methods
    const char* get_custom_text() const { return _custom_text; }
    bool has_active_custom_text() const { return _custom_text[0] != '\0' && millis() < _custom_text_expiration; }
};

#endif // IOTCLOUD_ENABLE_THERMOSTAT

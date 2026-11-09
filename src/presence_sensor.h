#pragma once

#ifdef IOTCLOUD_ENABLE_PRESENCE_SENSOR

#include <sensors/sensor.h>

class PRESENCE_SENSOR : public BaseSensor
{
private:
    const int _switch_pin;
    bool _last_state;
    bool _mem_switch_state_1 = false;
    unsigned long _last_debounce_time_1 = 0;
    unsigned long _setup_time = 0;
    const unsigned long START_DELAY = 5000;
    bool _presence_state = false;
    bool _initial_report_done = false;
    // Set when the broker may not have the current state yet: at boot, after
    // a change, and on every (re)connect. Cleared once a publish succeeds.
    bool _report_pending = true;
    bool report_presence(bool presence);
    void init(char *mqtt_header, EspMQTTClient *mqtt_client) override;
    void loop() override;

public:
    PRESENCE_SENSOR(
        const char *sensor_id,
        const char *sensor_name,
        const int switch_pin);
};

#endif

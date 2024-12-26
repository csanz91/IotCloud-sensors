#pragma once

#ifdef IOTCLOUD_ENABLE_PRESENCE_SENSOR

#include "notifier.h"

class PRESENCE_SENSOR : public NOTIFIER
{
private:
    const int _switch_pin;
    bool _last_state;
    bool _mem_switch_state_1 = false;
    unsigned long _last_debounce_time_1 = 0;
    unsigned long _setup_time = 0;
    const unsigned long START_DELAY = 5000;
    void report_presence(bool presence);
    void loop();

public:
    PRESENCE_SENSOR(
        const char *sensor_id,
        const char *sensor_name,
        const int switch_pin);

};

#endif // IOTCLOUD_ENABLE_PRESENCE_SENSOR

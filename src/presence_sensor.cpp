#ifdef IOTCLOUD_ENABLE_PRESENCE_SENSOR

#include "presence_sensor.h"

PRESENCE_SENSOR::PRESENCE_SENSOR(
    const char *sensor_id,
    const char *sensor_name,
    const int switch_pin)
    : NOTIFIER(sensor_id, sensor_name, "{}"),
      _switch_pin(switch_pin)
{
    pinMode(_switch_pin, INPUT_PULLUP);
    _last_state = digitalRead(_switch_pin);
    _setup_time = millis();
}


void PRESENCE_SENSOR::loop()
{
    const unsigned long now = millis();

    // Ignore the inputs during the first seconds. Avoid false triggers
    if (now - _setup_time < START_DELAY)
    {
        return;
    }

    // Debounce the switch
    bool switch_state = digitalRead(_switch_pin);
    if (switch_state != _last_state)
    {
        _mem_switch_state_1 = true;
        _last_debounce_time_1 = now;
    }

    if ((now - _last_debounce_time_1) > 50)
    {
        if (_mem_switch_state_1)
        {
            _mem_switch_state_1 = false;
            report_presence(switch_state);
        }
    }

    _last_state = switch_state;
}

void PRESENCE_SENSOR::report_presence(bool presence)
{
    send_notification(presence ? "true" : "false");
}

#endif // IOTCLOUD_ENABLE_PRESENCE_SENSOR

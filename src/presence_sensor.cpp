#include "presence_sensor.h"

PRESENCE_SENSOR::PRESENCE_SENSOR(
    const char *sensor_id,
    const char *sensor_name,
    const int switch_pin)
    : BaseSensor(sensor_id, sensor_name, "presence", "{}"),
      _switch_pin(switch_pin)
{
    pinMode(_switch_pin, INPUT_PULLUP);
    _last_state = digitalRead(_switch_pin);
    _setup_time = millis();
}

void PRESENCE_SENSOR::loop()
{
    const unsigned long now = millis();

    if (now - _setup_time < START_DELAY)
    {
        return;
    }

    if (!_initial_report_done) {
        _initial_report_done = true;
        _presence_state = digitalRead(_switch_pin);
        report_presence(_presence_state);
    }

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
            if (_presence_state != switch_state)
            {
                _presence_state = switch_state;
                report_presence(switch_state);
            }
        }
    }

    _last_state = switch_state;
}

void PRESENCE_SENSOR::report_presence(bool presence)
{
    if (!_mqtt_client || !_mqtt_client->isConnected())
    {
        return;
    }
    char constructedTopic[94] = "";
    construct_topic(constructedTopic, "state");
    _mqtt_client->publish(constructedTopic, presence ? "true" : "false", true);
}
#ifdef IOTCLOUD_ENABLE_SERVO

#include "servo_button.h"

SERVO_BUTTON::SERVO_BUTTON(
    const char *sensor_id,
    const char *sensor_name,
    const char *metadata,
    const unsigned long activation_time,
    const unsigned long activation_angle,
    const unsigned long resting_angle,
    const int pin) : Toggle(sensor_id,
                            sensor_name),
                     _activation_time(activation_time),
                     _activation_angle(activation_angle),
                     _resting_angle(resting_angle)
{
    _servo = new Servo();
    _servo->attach(pin);
    push_button(false); // Go to the resting position
}

void SERVO_BUTTON::push_button(bool state)
{
    Serial.println("push_button: TOGGLE");
    _servo->write(state ? _activation_angle : _resting_angle);
}

void SERVO_BUTTON::set_toggle_state(ToggleState toggle_state)
{
    if (toggle_state == ToggleState::TOGGLE)
    {
        push_button(true);
        Serial.println("SERVO_BUTTON: TOGGLE");
        _last_toggled = millis();
    }
}

void SERVO_BUTTON::loop()
{
    if (_last_toggled > 0 && millis() - _last_toggled > _activation_time)
    {
        push_button(false);
        _last_toggled = 0;
    }
}

#endif // IOTCLOUD_ENABLE_SERVO
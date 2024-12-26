#pragma once

#ifdef IOTCLOUD_ENABLE_SERVO

#include <sensors/toogle.h>
#include <Servo.h>

class SERVO_BUTTON : public Toogle
{

private:
    const long _activation_time;
    const long _activation_angle;
    const long _resting_angle;
    unsigned long _last_toogled = 0;
    Servo *_servo;
    void push_button(bool state);
    void set_toogle_state(ToogleStates);
    void loop();

public:
    SERVO_BUTTON(
        const char *sensor_id,
        const char *sensor_name,
        const char *metadata,
        const unsigned long activation_time,
        const unsigned long activation_angle,
        const unsigned long resting_angle,
        const int pin);
};

#endif // IOTCLOUD_ENABLE_SERVO
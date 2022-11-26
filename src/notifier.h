#pragma once

#include <sensors/sensor.h>

class NOTIFIER : public BaseSensor
{
public:
    void send_notification(const char *payload);

private:
    EspMQTTClient *_mqtt_client;
    void init(char *mqtt_header, EspMQTTClient *mqtt_client);

public:
    NOTIFIER(
        const char *sensor_id,
        const char *sensor_name,
        const char *metadata);
};
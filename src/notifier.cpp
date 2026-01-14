#include "notifier.h"

NOTIFIER::NOTIFIER(
    const char *sensor_id,
    const char *sensor_name,
    const char *metadata) : BaseSensor(sensor_id,
                                       sensor_name,
                                       "notifier",
                                       metadata,
                                       "v1.0")
{
}

void NOTIFIER::init(char *mqtt_header, EspMQTTClient *mqtt_client)
{
    BaseSensor::init(mqtt_header, mqtt_client);

    _mqtt_client = mqtt_client;
}

void NOTIFIER::send_notification(const char *payload)
{
    if (!_mqtt_client)
        return;

    char constructedTopic[128] = "";
    construct_topic(constructedTopic, sizeof(constructedTopic), "aux/notification");
    _mqtt_client->publish(constructedTopic, payload);
}

#pragma once

#ifdef IOTCLOUD_ENABLE_WIFI_RSSI

#include <sensors/analog.h>

class IotCloud_Wifi_RSSI : public AnalogSensor
{

private:
    void get_value();

public:
    IotCloud_Wifi_RSSI(
        const char *sensor_id,
        const char *sensor_name);
};

#endif // IOTCLOUD_ENABLE_WIFI_RSSI
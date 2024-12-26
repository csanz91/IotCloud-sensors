#pragma once

#ifdef IOTCLOUD_ENABLE_CO2_SENSOR

#include <sensors/analog.h>
#include <MHZ19.h>
#ifdef ESP8266
#include <SoftwareSerial.h>
#endif

class IotCloud_CO2 : public AnalogSensor
{
private:
    #ifdef ESP8266
    SoftwareSerial *_serial;
    #else
    HardwareSerial *_serial;
    #endif
    MHZ19 *_co2_sensor;
    void get_value();
    void init(char *mqtt_header, EspMQTTClient *mqtt_client);

public:

public:
    IotCloud_CO2(
        const char *sensor_id,
        const char *sensor_name,
        const char *metadata,
        const int rx, const int tx);
};

#endif // IOTCLOUD_ENABLE_CO2_SENSOR
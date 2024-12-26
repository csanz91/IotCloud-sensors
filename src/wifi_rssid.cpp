#ifdef IOTCLOUD_ENABLE_WIFI_RSSI

#include "wifi_rssid.h"

IotCloud_Wifi_RSSI::IotCloud_Wifi_RSSI(
    const char *sensor_id,
    const char *sensor_name) : AnalogSensor(sensor_id,
                                            sensor_name,
                                            30000,
                                            0,
                                            false,
                                            "analog",
                                            "{\"engUnits\": \"dBm\", \"stringFormat\": \"0\"}")
{
}

void IotCloud_Wifi_RSSI::get_value()
{

    if (WiFi.status() == WL_CONNECTED)
    {
        long rssi = WiFi.RSSI();
        set_value(rssi);
    }
}

#endif // IOTCLOUD_ENABLE_WIFI_RSSI
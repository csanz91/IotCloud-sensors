# IotCloud Sensors Library

[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)
[![Platform](https://img.shields.io/badge/Platform-ESP8266%20%7C%20ESP32-brightgreen.svg)](https://www.espressif.com/)
[![Version](https://img.shields.io/badge/Version-1.0.0-orange.svg)](https://github.com/csanz91/IotCloud-sensors)

A comprehensive Arduino library providing specialized sensor implementations for the **IotCloud** home automation platform. This library extends the base sensor classes from the [IotCloud-arduino-lib](https://github.com/csanz91/IotCloud-arduino-lib) to provide ready-to-use sensor modules for various IoT applications.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Dependencies](#dependencies)
- [Supported Sensors](#supported-sensors)
  - [CO2 Sensor (MH-Z19)](#co2-sensor-mh-z19)
  - [Temperature Sensor (DS18B20)](#temperature-sensor-ds18b20)
  - [Temperature & Humidity (SI7021/AM2301/DHT)](#temperature--humidity-si7021am2301dht)
  - [Thermostat Subsensor](#thermostat-subsensor)
  - [Presence Sensor](#presence-sensor)
  - [Servo Button](#servo-button)
  - [WiFi RSSI Monitor](#wifi-rssi-monitor)
  - [Notifier](#notifier)
- [Architecture](#architecture)
- [Configuration](#configuration)
- [Usage Examples](#usage-examples)
- [MQTT Topics](#mqtt-topics)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- **Modular Design**: Each sensor type is encapsulated in its own class with a clean API
- **Conditional Compilation**: Each sensor is opt-in with an `IOTCLOUD_ENABLE_*` build flag, e.g. `build_flags = -DIOTCLOUD_ENABLE_DS18B20` in `platformio.ini`. It must be a build flag: a `#define` in the sketch doesn't reach the sensor's `.cpp` file
- **Multi-Platform Support**: Compatible with both ESP8266 and ESP32 microcontrollers
- **MQTT Integration**: Seamless integration with the IotCloud MQTT-based home automation platform
- **Filtering & Smoothing**: Built-in value filtering with configurable rate-of-change limits to reject spurious readings
- **Low Power Friendly**: Non-blocking design with periodic reporting intervals

---

## Installation

### Arduino Library Manager

1. Open the Arduino IDE
2. Navigate to **Sketch** → **Include Library** → **Manage Libraries...**
3. Search for "IotCloud Sensors"
4. Click **Install**

### Manual Installation

1. Download or clone this repository
2. Copy the `IotCloud-sensors` folder to your Arduino libraries directory:
   - **Windows**: `Documents/Arduino/libraries/`
   - **macOS**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
3. Restart the Arduino IDE

---

## Dependencies

This library requires the following dependencies:

### Required

| Dependency | Description | Installation |
|------------|-------------|--------------|
| [IotCloud-arduino-lib](https://github.com/csanz91/IotCloud-arduino-lib) | Core IotCloud library with base sensor classes | Manual install |
| [EspMQTTClient](https://github.com/plapointe6/EspMQTTClient) | MQTT client wrapper for ESP8266/ESP32 | Library Manager |

### Optional (sensor-specific)

| Dependency | Required For | Installation |
|------------|--------------|--------------|
| [MH-Z19](https://github.com/WifWaf/MH-Z19) | CO2 Sensor | Library Manager |
| [OneWire](https://www.pjrc.com/teensy/td_libs_OneWire.html) | DS18B20 Temperature Sensor | Library Manager |
| [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) | DS18B20 Temperature Sensor | Library Manager |
| [Servo](https://www.arduino.cc/reference/en/libraries/servo/) | Servo Button | Built-in |

---

## Supported Sensors

### CO2 Sensor (MH-Z19)

**File**: `co2_sensor.h` / `co2_sensor.cpp`
**Enable Flag**: `IOTCLOUD_ENABLE_CO2_SENSOR`

Monitors CO2 concentration in parts per million (PPM) using the MH-Z19 NDIR sensor module.

#### Features

- UART communication (SoftwareSerial on ESP8266, HardwareSerial on ESP32)
- Auto-calibration control (disabled by default for stability)
- Built-in MH-Z19 filtering enabled
- Remote calibration trigger via MQTT (`aux/calibrate` topic)
- 3% rate-of-change filter to reject spurious readings

#### Constructor

```cpp
IotCloud_CO2(
    const char *sensor_id,     // Unique identifier for the sensor
    const char *sensor_name,   // Human-readable name
    const char *metadata,      // JSON metadata (units, format, etc.)
    const int rx,              // RX pin for serial communication
    const int tx               // TX pin for serial communication
);
```

#### Example

```cpp
// Needs the build flag -DIOTCLOUD_ENABLE_CO2_SENSOR (a #define here would not reach the sensor's .cpp)
#include <co2_sensor.h>

IotCloud_CO2 co2_sensor("co2_living_room", "Living Room CO2", 
                         "{\"engUnits\": \"ppm\", \"stringFormat\": \"0\"}", 
                         D5, D6);
```

---

### Temperature Sensor (DS18B20)

**File**: `ds18b20.h` / `ds18b20.cpp`
**Enable Flag**: `IOTCLOUD_ENABLE_DS18B20`

Digital temperature sensor using the Dallas DS18B20 one-wire protocol.

#### Features

- 12-bit resolution for high accuracy (±0.0625°C)
- Non-blocking temperature conversion
- Automatic device discovery on the bus
- State machine design for efficient polling

#### Constructor

```cpp
DS18B20(const int one_wire_bus);  // GPIO pin for the OneWire bus
```

#### Usage

```cpp
// Needs the build flag -DIOTCLOUD_ENABLE_DS18B20 (a #define here would not reach the sensor's .cpp)
#include <ds18b20.h>

DS18B20 temp_sensor(D4);

void loop() {
    float temperature = temp_sensor.get_value();
    if (temperature != NULL) {
        // Temperature is ready
    }
}
```

#### State Machine

The DS18B20 class uses a three-step state machine:

1. **GET_ADDR**: Searches for a sensor on the bus
2. **REQ_TEMP**: Requests a temperature conversion
3. **GET_TEMP**: Reads the converted temperature

---

### Temperature & Humidity (SI7021/AM2301/DHT)

**File**: `si7021_am2301.h` / `si7021_am2301.cpp`
**Enable Flag**: None (always available)

Multi-protocol temperature and humidity sensor supporting various DHT-compatible sensors.

#### Supported Models

| Model | Timing | Use Case |
|-------|--------|----------|
| DHT11 | 18ms | Basic applications |
| DHT12 | 200ms | I2C compatible |
| DHT22 | 2ms | Higher precision |
| AM2301 | 900µs | Industrial grade |
| SI7021 | 500µs | High accuracy |

#### Features

- Bit-banging protocol implementation
- Checksum verification for data integrity
- NaN detection for invalid readings
- Interrupt-safe reading with `noInterrupts()`/`interrupts()`

#### Constructor

```cpp
SI7021_AM2301(
    const int sensor_pin,              // GPIO pin connected to the sensor
    const SensorModel sensor_model     // DHT11, DHT12, DHT22, AM2301, or SI7021
);
```

#### Example

```cpp
#include <si7021_am2301.h>

SI7021_AM2301 sensor(D2, SI7021_AM2301::SensorModel::SI7021);

void loop() {
    sensor.read_values();
    if (!isnan(sensor.temperature)) {
        Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", 
                      sensor.temperature, sensor.humidity);
    }
}
```

---

### Thermostat Subsensor

**File**: `thermostat_subsensor.h` / `thermostat_subsensor.cpp`
**Enable Flag**: None (always available)

A specialized sensor for thermostat systems that publishes temperature and humidity data to a main thermostat's MQTT topic, enabling multi-zone temperature monitoring.

#### Features

- **Topic Impersonation**: Publishes to main thermostat's topic for seamless integration
- **Display Callback**: Optional callback for updating external displays
- **Value Filtering**: Built-in rate-of-change filtering (default 10%) to reject sudden spikes
- **Dual Publication**: Publishes both temperature (`value`) and humidity (`aux/humidity`)

#### Constructor

```cpp
ThermostatSubsensor(
    const char *sensor_id,       // This sensor's ID (for its own metadata/registration)
    const char *sensor_name,     // Human-readable name
    const char *subsensor_id,    // Main thermostat's sensor ID (for impersonation)
    const int sensor_pin,        // GPIO pin for SI7021/DHT sensor
    const SensorModel model,     // SI7021_AM2301::SensorModel (default: SI7021)
    const unsigned int report_period,  // Report interval in ms (default: 30000)
    const float filter_rate      // Rate-of-change filter (default: 0.1 = 10%)
);
```

#### Display Callback

```cpp
void display_update(float temperature, float humidity) {
    display.showTemperature(temperature);
    display.showHumidity(humidity);
}

ThermostatSubsensor subsensor("subsensor_1", "Bedroom Temp", "thermostat_main", D2);
subsensor.set_display_callback(display_update);
```

#### MQTT Topics Published

| Topic | Value | Retained |
|-------|-------|----------|
| `{header}/{subsensor_id}/value` | Temperature | Yes |
| `{header}/{subsensor_id}/aux/humidity` | Humidity | Yes |

---

### Presence Sensor

**File**: `presence_sensor.h` / `presence_sensor.cpp`
**Enable Flag**: None (always available)

PIR motion sensor or binary presence detector with debouncing.

#### Features

- Software debouncing (50ms) to prevent false triggers
- 5-second startup delay to allow sensor stabilization
- Initial state report after startup delay
- Retained MQTT messages for state persistence

#### Constructor

```cpp
PRESENCE_SENSOR(
    const char *sensor_id,
    const char *sensor_name,
    const int switch_pin        // GPIO pin with INPUT_PULLUP
);
```

#### Example

```cpp
#include <presence_sensor.h>

PRESENCE_SENSOR pir("pir_office", "Office Motion", D3);
```

#### MQTT

Publishes to `{header}/{sensor_id}/state` with values `"true"` or `"false"`.

---

### Servo Button

**File**: `servo_button.h` / `servo_button.cpp`
**Enable Flag**: `IOTCLOUD_ENABLE_SERVO`

Actuates a physical button using a servo motor, useful for controlling devices that only have physical buttons.

#### Features

- Configurable activation angle and resting angle
- Adjustable activation duration
- Automatic return to resting position after activation

#### Constructor

```cpp
SERVO_BUTTON(
    const char *sensor_id,
    const char *sensor_name,
    const char *metadata,
    const unsigned long activation_time,   // Duration of button press in ms
    const unsigned long activation_angle,  // Servo angle when pressed
    const unsigned long resting_angle,     // Servo angle when idle
    const int pin                          // Servo signal pin
);
```

#### Example

```cpp
// Needs the build flag -DIOTCLOUD_ENABLE_SERVO (a #define here would not reach the sensor's .cpp)
#include <servo_button.h>

// Press for 500ms, rotate from 90° (rest) to 45° (press)
SERVO_BUTTON button("ac_button", "AC Power Button", "{}", 500, 45, 90, D6);
```

---

### WiFi RSSI Monitor

**File**: `wifi_rssid.h` / `wifi_rssid.cpp`
**Enable Flag**: `IOTCLOUD_ENABLE_WIFI_RSSI`

Monitors and reports WiFi signal strength (RSSI) in dBm.

#### Features

- Periodic reporting (default: 30 seconds)
- Only reports when WiFi is connected
- Integer values (no decimals)

#### Constructor

```cpp
IotCloud_Wifi_RSSI(
    const char *sensor_id,
    const char *sensor_name
);
```

#### Example

```cpp
// Needs the build flag -DIOTCLOUD_ENABLE_WIFI_RSSI (a #define here would not reach the sensor's .cpp)
#include <wifi_rssid.h>

IotCloud_Wifi_RSSI rssi_sensor("wifi_signal", "WiFi Signal Strength");
```

---

### Notifier

**File**: `notifier.h` / `notifier.cpp`
**Enable Flag**: None (always available)

Sends custom notification payloads via MQTT for integration with notification services.

#### Constructor

```cpp
NOTIFIER(
    const char *sensor_id,
    const char *sensor_name,
    const char *metadata
);
```

#### Usage

```cpp
#include <notifier.h>

NOTIFIER notifier("alerts", "System Alerts", "{}");

// Send a notification
notifier.send_notification("{\"title\": \"Alert\", \"message\": \"Door opened\"}");
```

#### MQTT Topic

Publishes to `{header}/{sensor_id}/aux/notification`.

---

## Architecture

```
IotCloud-sensors Library
├── Extends IotCloud-arduino-lib
│   ├── BaseSensor (sensor.h)
│   ├── AnalogSensor (analog.h)
│   └── Toogle (toogle.h)
│
├── Sensor Implementations
│   ├── IotCloud_CO2        → extends AnalogSensor
│   ├── ThermostatSubsensor → extends AnalogSensor
│   ├── IotCloud_Wifi_RSSI  → extends AnalogSensor
│   ├── SERVO_BUTTON        → extends Toogle
│   ├── PRESENCE_SENSOR     → extends BaseSensor
│   └── NOTIFIER            → extends BaseSensor
│
└── Standalone Utilities
    ├── DS18B20             (standalone class)
    └── SI7021_AM2301       (standalone class)
```

### Class Hierarchy

| Class | Parent | Purpose |
|-------|--------|---------|
| `IotCloud_CO2` | `AnalogSensor` | CO2 monitoring with MQTT |
| `ThermostatSubsensor` | `AnalogSensor` | Thermostat temp/humidity |
| `IotCloud_Wifi_RSSI` | `AnalogSensor` | WiFi signal monitoring |
| `SERVO_BUTTON` | `Toogle` | Servo-actuated button |
| `PRESENCE_SENSOR` | `BaseSensor` | Binary presence detection |
| `NOTIFIER` | `BaseSensor` | MQTT notification sender |
| `DS18B20` | — | DS18B20 temperature reading |
| `SI7021_AM2301` | — | DHT/SI7021 temp/humidity |

---

## Configuration

### Compile-Time Flags

Enable only the sensors you need to optimize memory usage:

// In platformio.ini:
// To ENABLE sensors:
#define IOTCLOUD_ENABLE_CO2_SENSOR    // Enable MH-Z19 CO2 sensor
#define IOTCLOUD_ENABLE_DS18B20       // Enable DS18B20
#define IOTCLOUD_ENABLE_SERVO         // Enable Servo
#define IOTCLOUD_ENABLE_WIFI_RSSI     // Enable WiFi RSSI
#define IOTCLOUD_ENABLE_THERMOSTAT    // Enable Thermostat Subsensor
#define IOTCLOUD_ENABLE_NOTIFIER      // Enable Notifier
#define IOTCLOUD_ENABLE_PRESENCE_SENSOR // Enable Presence Sensor
#define IOTCLOUD_ENABLE_SI7021        // Enable SI7021/DHT Sensor

---

## Usage Examples

### Complete Example: Multi-Sensor Device

```cpp
#include <Arduino.h>
#include <iotcloud.h>

// Enable sensors before including headers
// Needs the build flag -DIOTCLOUD_ENABLE_CO2_SENSOR (a #define here would not reach the sensor's .cpp)
// Needs the build flag -DIOTCLOUD_ENABLE_WIFI_RSSI (a #define here would not reach the sensor's .cpp)

#include <co2_sensor.h>
#include <wifi_rssid.h>
#include <presence_sensor.h>
#include <thermostat_subsensor.h>

// Define sensors
IotCloud_CO2 co2("co2", "CO2 Sensor", "{}", D5, D6);
IotCloud_Wifi_RSSI wifi_rssi("wifi", "WiFi Signal");
PRESENCE_SENSOR presence("motion", "Motion Sensor", D3);
ThermostatSubsensor thermo("temp_bedroom", "Bedroom", "thermostat_main", D2);

// Register sensors with IotCloud
BaseSensor* sensors[] = {&co2, &wifi_rssi, &presence, &thermo};
const int sensor_count = sizeof(sensors) / sizeof(sensors[0]);

void setup() {
    Serial.begin(115200);
    
    // Initialize IotCloud...
    // iotcloud.init(...);
    
    // Register sensors
    for (int i = 0; i < sensor_count; i++) {
        // sensors[i]->init(mqtt_header, mqtt_client);
    }
}

void loop() {
    // IotCloud loop handles MQTT and sensor updates
    // iotcloud.loop();
}
```

---

## MQTT Topics

All sensors publish to topics following this pattern:

```
{mqtt_header}/{sensor_id}/{endpoint}
```

| Sensor | Endpoint | Description |
|--------|----------|-------------|
| Analog Sensors | `value` | Current sensor value |
| CO2 Sensor | `aux/calibrate` | Calibration trigger (subscribe) |
| Presence | `state` | `"true"` or `"false"` |
| Notifier | `aux/notification` | JSON notification payload |
| ThermostatSubsensor | `value` | Temperature |
| ThermostatSubsensor | `aux/humidity` | Humidity |

---

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-sensor`)
3. Commit your changes (`git commit -am 'Add new sensor support'`)
4. Push to the branch (`git push origin feature/new-sensor`)
5. Open a Pull Request

### Coding Guidelines

- Follow the existing code style
- Use `#pragma once` for header guards
- Wrap optional features in `#ifdef` guards
- Document public methods with comments
- Test on both ESP8266 and ESP32

---

## License

This library is licensed under the **GNU Lesser General Public License v2.1 (LGPL-2.1)**.

See the [LICENSE](LICENSE) file for full details.

---

## Author

**Cesar Sanz**
- Email: cesarsanz.91@gmail.com
- GitHub: [@csanz91](https://github.com/csanz91)

---

## Related Projects

- [IotCloud-arduino-lib](https://github.com/csanz91/IotCloud-arduino-lib) - Core IotCloud Arduino library
- [IotCloud-devices](https://github.com/csanz91/IotCloud-devices) - Device firmware implementations

# Firmware Setup

The main ESP32 program is `mobile_substation_monitoring.ino`.

## Arduino IDE Libraries

Install the following libraries before compiling:

- Blynk
- DHT sensor library
- Adafruit Unified Sensor (dependency commonly used by the DHT library)
- PZEM004Tv30 by Jakub Mandula / mandulaj

ESP32 board support must also be installed in Arduino IDE.

## Local Secret File

1. Copy `secrets.example.h`.
2. Rename the copy to `secrets.h`.
3. Enter your real Wi-Fi and Blynk credentials.
4. Do not commit `secrets.h` to GitHub.

## Current Pin Plan

| ESP32 | Function |
|---|---|
| GPIO 2 | DHT11 data |
| GPIO 12 | UART RX from PZEM TX |
| GPIO 13 | UART TX to PZEM RX |

The ESP32 and PZEM communication uses `Serial2` hardware UART.

## Firmware Functions

The current firmware performs:

- DHT11 temperature measurement
- DHT11 humidity measurement
- PZEM-004T voltage measurement
- PZEM-004T current measurement
- PZEM-004T power, energy, frequency and power-factor measurement
- Threshold checking
- Short-window trend detection
- Rule-based anomaly detection
- Normal / Warning / Critical condition classification
- Blynk dashboard transmission
- Serial Monitor diagnostic output

## Important Note

The threshold values are prototype values for laboratory testing. They are not relay settings or protection settings for a real 33/11 kV mobile substation.

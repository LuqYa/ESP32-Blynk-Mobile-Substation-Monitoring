# Firmware Setup

The Arduino sketch folder is `firmware/`.

Open this file in Arduino IDE:

`firmware/firmware.ino`

Arduino requires the primary `.ino` file to match the sketch-folder name. The implementation remains in:

`firmware/mobile_substation_monitoring.ino`

Both files are compiled together as one sketch.

## Arduino IDE Libraries

Install the following libraries before compiling:

- Blynk
- DHT sensor library
- Adafruit Unified Sensor
- PZEM004Tv30 by Jakub Mandula / mandulaj

ESP32 board support must also be installed in Arduino IDE.

## Local Secret File

1. Copy `secrets.example.h`.
2. Rename the copy to `secrets.h`.
3. Enter your real Wi-Fi and Blynk credentials.
4. Do not commit `secrets.h` to GitHub.

Required local values:

```cpp
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Mobile Substation Monitoring"
```

`BRIDGE_SHARED_SECRET` is optional and is only required later if OpenAI AI-mode authentication is enabled on Render.

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
- Blynk V0-V10 dashboard transmission
- Direct HTTPS request to Render when WARNING, CRITICAL, sensor fault or trend anomaly occurs
- Render advisory result written to Blynk V11 and V12
- Serial Monitor diagnostic output

## GitHub Compile Check

The repository includes `.github/workflows/firmware-build.yml`.

Every firmware change is automatically compiled in GitHub Actions using Arduino CLI, the ESP32 core and the required libraries. The workflow generates dummy credentials only for compilation and never uses your real Wi-Fi or Blynk secrets.

## Important Note

The threshold values are prototype values for laboratory testing. They are not relay settings or protection settings for a real 33/11 kV mobile substation.

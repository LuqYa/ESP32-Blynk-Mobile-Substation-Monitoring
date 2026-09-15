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

The current V0–V11 firmware does not make automatic Render requests; the Render advisory endpoint remains independently available.

## Current Pin Plan

| ESP32 | Function |
|---|---|
| GPIO 2 | DHT11 data |
| GPIO 13 | UART RX from PZEM TX |
| GPIO 12 | UART TX to PZEM RX |

The ESP32 and PZEM communication uses `Serial2` hardware UART.

## Firmware Functions

The current firmware reads DHT11 temperature/humidity and PZEM AC voltage/current. It publishes V0–V11, updates sensor LED colors, and implements four-reading trends, distinct-parameter counting, SYSTEM FAULT priority, three-normal-reading recovery and four event codes.

Decision logic is in `monitoring_logic.h`, shared with host behavior tests. The sketch retains its existing folder and entry point. Monitoring starts without waiting indefinitely for Wi-Fi/Blynk; connection attempts are bounded and runtime colors refresh after reconnect.

See [Blynk setup](../blynk/dashboard_setup.md) and [thresholds](../testing/thresholds.md).

## GitHub Compile Check

The repository includes `.github/workflows/firmware-build.yml`.

Every firmware change is automatically compiled in GitHub Actions using Arduino CLI, the ESP32 core and the required libraries. The workflow generates dummy credentials only for compilation and never uses your real Wi-Fi or Blynk secrets.

## Important Note

The threshold values are prototype values for laboratory testing. They are not relay settings or protection settings for a real 33/11 kV mobile substation.

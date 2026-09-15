# Hardware Connection Summary

This file summarises the planned laboratory-scale hardware connections for the prototype. It is not a high-voltage wiring drawing.

## ESP32 to DHT11

| ESP32 | DHT11 | Function |
|---|---|---|
| 3V3 | VCC | Sensor supply |
| GND | GND | Common ground |
| D2 / GPIO 2 | DOUT | Temperature and humidity data |

## ESP32 to PZEM-004T

| ESP32 | PZEM-004T | Function |
|---|---|---|
| VIN / 5 V supply as appropriate | 5V | Module supply |
| GND | GND | Common ground |
| D13 / GPIO 13 | TX | ESP32 receives PZEM data |
| D12 / GPIO 12 | RX | ESP32 transmits to PZEM |

> RX/TX are crossed: ESP32 RX connects to PZEM TX, while ESP32 TX connects to PZEM RX.

## PZEM-004T and Current Transformer

The PZEM-004T measures AC electrical quantities using its voltage input and current-transformer arrangement. The current transformer must be installed only according to the manufacturer's instructions and under a safe laboratory setup.

## Communication Path

```text
DHT11 ---------\
                \
                 > ESP32 ---- Wi-Fi ---- Blynk Cloud ---- Phone/Web Dashboard
                /
PZEM-004T + CT-/
```

## Prototype Safety Scope

- Do not connect this prototype directly to energized 33 kV or 11 kV equipment.
- Use safe isolated or simulated laboratory signals.
- Keep the ESP32 and sensor-side electronics electrically separated from hazardous circuits.
- Verify the PZEM module wiring before energising any laboratory AC source.
- Use supervision and suitable protection when working with mains-voltage test circuits.

## Planned Monitoring Parameters

- Temperature
- Relative humidity
- AC voltage
- AC current
- Operating condition: NORMAL / WARNING / CRITICAL / SYSTEM FAULT
- Trend/anomaly status

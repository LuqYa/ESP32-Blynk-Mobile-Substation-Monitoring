# Hardware

## Prototype Components

- ESP32 development board
- DHT11 temperature and humidity sensor
- PZEM-004T electrical monitoring module
- Current transformer (CT)
- Suitable isolated laboratory supply and supporting components

## Current Connection Summary

### DHT11 to ESP32

- ESP32 3V3 -> DHT11 VCC
- ESP32 GND -> DHT11 GND
- ESP32 D2 -> DHT11 DOUT

### PZEM-004T to ESP32

Current prototype pin plan:

- ESP32 VIN / suitable 5 V supply -> PZEM 5V
- ESP32 GND -> PZEM GND
- ESP32 D13 -> PZEM RX
- ESP32 D12 -> PZEM TX
- PZEM-004T -> Current Transformer (CT)

Verify the exact PZEM-004T model, voltage levels, UART direction, isolation requirements, and manufacturer documentation before energizing the prototype.

## Safety

Do not directly connect the ESP32, DHT11, or low-voltage prototype wiring to energized 33 kV or 11 kV equipment. The FYP prototype should use safe isolated signals and simulated operating conditions.

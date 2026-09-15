# Wiring

This page documents the physical connections between the ESP32 and the 64×32
HUB75 RGB LED matrix used in the LUMEN project.

---

## ESP32 → HUB75

The ESP32 is connected to the HUB75 input connector on the LED matrix.

The connection consists of:

- RGB data lines
- Row address lines
- Clock
- Latch
- Output Enable
- Ground

### Pin Mapping

| HUB75 Signal | ESP32 GPIO | Function |
|---|---:|---|
| R1 | GPIO 25 | Upper-half red data |
| G1 | GPIO 26 | Upper-half green data |
| B1 | GPIO 27 | Upper-half blue data |
| R2 | GPIO 14 | Lower-half red data |
| G2 | GPIO 12 | Lower-half green data |
| B2 | GPIO 13 | Lower-half blue data |
| A | GPIO 23 | Row address |
| B | GPIO 19 | Row address |
| C | GPIO 5 | Row address |
| D | Not used | Not required for this panel |
| E | Not used | Not required for this panel |
| LAT | GPIO 4 | Data latch |
| OE | GPIO 15 | Output enable |
| CLK | GPIO 16 | Pixel clock |
| GND | GND | Common ground |
---

## HUB75 Data Flow

The basic signal path is:

```text
ESP32
  │
  │ RGB + control signals
  ▼
HUB75 INPUT
  │
  ▼
LED Driver Circuitry
  │
  ▼
64 × 32 RGB LED Matrix
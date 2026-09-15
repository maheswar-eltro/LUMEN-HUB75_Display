# Hardware Components

This page documents the main hardware used to build the LUMEN LED matrix display.

---

## 1. ESP32

The ESP32 is the main microcontroller used to control the LED matrix.

It handles:

- Communication with the HUB75 LED matrix
- Sending RGB pixel data
- Running display animations
- Rendering scrolling text
- Controlling the display through the configured HUB75 interface

The ESP32 was chosen as the controller because the project requires considerably
more display-processing capability than a basic Arduino LED project.

---

## 2. 64×32 RGB LED Matrix Panel

The main display is a **64×32 RGB LED matrix panel** using a HUB75 interface.

### Specifications

| Property | Details |
|---|---|
| Resolution | 64 × 32 pixels |
| Total pixels | 2,048 |
| Interface | HUB75 |
| Scan type | 1/8 scan (8S) |
| LED package | SMD2525 |
| Driver IC | SM16208SF |

The panel contains:

**64 × 32 = 2,048 RGB LED pixels**

Because the panel uses a **1/8 scan architecture**, the LEDs are multiplexed
rather than being driven as 2,048 independently active LEDs at the same time.

---

## 3. HUB75 Interface

The LED matrix is controlled through its HUB75 interface.

The interface carries the signals required to control:

- Red channel data
- Green channel data
- Blue channel data
- Row selection
- Clock
- Latch
- Output enable

The ESP32 sends the required display data through these signals to refresh
the matrix.

---

## 4. SM16208SF Driver ICs

The LED matrix uses **SM16208SF** driver ICs.

These ICs are part of the panel's internal LED-driving circuitry and handle
the electrical driving of the LED data received through the matrix interface.

The presence of these ICs was identified while investigating the panel hardware
during the initial setup.

---

## 5. SMD2525 LEDs

The panel uses **SMD2525** LED packages.

These LEDs form the RGB pixels across the 64×32 display.

---

## 6. Power Supply

The LED matrix requires an external power source capable of supplying the
current required by the panel.

The ESP32 and LED matrix are powered as part of the overall display setup.

Currently: 5V 10A Power Bank is being used.

---

## 7. Jumper / Connection Wires

Wires are used to connect the ESP32 to the HUB75 input of the LED matrix.

They carry the display-control and data signals between the controller and
the panel.

---

## Hardware Summary

| Component | Role |
|---|---|
| ESP32 | Main controller |
| 64×32 RGB LED Matrix | Display |
| HUB75 interface | Communication interface |
| SM16208SF | LED driver circuitry |
| SMD2525 LEDs | RGB LED pixels |
| Power supply | Powers the display |
| Jumper wires | Electrical connections |

---

## System Overview

```text
             ┌─────────────────┐
             │      ESP32      │
             │  Microcontroller│
             └────────┬────────┘
                      │
                      │ HUB75
                      │
                      ▼
          ┌─────────────────────────┐
          │     64 × 32 RGB        │
          │      LED MATRIX        │
          │                         │
          │     2048 Pixels        │
          │      1/8 Scan          │
          └─────────────────────────┘
                      │
                      │
                Power Supply
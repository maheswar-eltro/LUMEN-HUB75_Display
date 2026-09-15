# LUMEN — Project Progress

## Project Overview

LUMEN is an ESP32-controlled RGB LED matrix display project developed for the Electronics Society.

The project uses a 64×32 RGB LED matrix panel with a HUB75 interface and 1/8 scan (8S) configuration.

The main goal of the project is to create a programmable display capable of showing text, colours, animations, and other visual effects.

---

## Progress Timeline

### 1. Project Setup

**Status:** Done

- Project repository created.
- Repository structure organized into:
  - `documentation/`
  - `hardware/`
  - `media/`
  - `src/`
  - `README.md`

---

### 2. LED Matrix Panel Setup

**Status:** Done

- 64×32 RGB LED matrix panel obtained.
- Panel identified as an 8S / 1/8 scan display.
- HUB75 interface identified and used for communication with the ESP32.
- Panel uses SMD2525 LEDs.
- Driver ICs identified as SM16208SF.

### Panel Specifications

| Parameter | Value |
|---|---|
| Resolution | 64×32 |
| Total Pixels | 2,048 |
| Scan Type | 1/8 scan (8S) |
| Interface | HUB75 |
| LED Type | SMD2525 |
| Driver IC | SM16208SF |
| Controller | ESP32 |

---

### 3. ESP32 Controller Setup

**Status:** Done

- ESP32 selected as the main controller.
- HUB75 interface connected to the ESP32.
- GPIO pins assigned for RGB data, row addressing, clock, latch, and output enable.
- Common ground established between the ESP32 and LED matrix.

### GPIO Configuration

| HUB75 Signal | ESP32 GPIO |
|---|---:|
| R1 | GPIO 25 |
| G1 | GPIO 26 |
| B1 | GPIO 27 |
| R2 | GPIO 14 |
| G2 | GPIO 12 |
| B2 | GPIO 13 |
| A | GPIO 23 |
| B | GPIO 19 |
| C | GPIO 5 |
| D | Not used |
| E | Not used |
| LAT | GPIO 4 |
| OE | GPIO 15 |
| CLK | GPIO 16 |
| GND | GND |

---

### 4. Initial Display Testing

**Status:** Done

The first stage of software development involved testing communication between the ESP32 and the LED matrix.

The display was tested using full-screen colours and basic output.

Tests included:

- Red
- Green
- Blue
- Cyan
- Magenta
- Yellow
- White
- Screen clearing

These tests were used to verify the HUB75 connection and basic display operation.

---

### 5. 1/8 Scan Mapping

**Status:** Done

The LED matrix uses a 1/8 scan (8S) architecture.

The physical arrangement of the LEDs does not directly correspond to a normal 64×32 coordinate system. Because of this, custom coordinate mapping was required.

A custom `EightScanPanel` class was implemented using `VirtualMatrixPanel`.

A custom `getCoords()` function was then used to remap the display coordinates for the 1/8 scan panel.

This allows the software to address the display as a logical 64×32 screen while handling the physical scanning arrangement.

---

### 6. HUB75 Wiring Debugging

**Status:** Done

During initial testing, the display produced incorrect patterns and box-like artifacts.

The following connections were checked:

- RGB data connections
- Row-address connections
- Clock
- Latch
- Output Enable
- Ground

The problem was eventually traced to the physical wiring.

After correcting the wiring, the display output became stable and the panel displayed the intended content correctly.

This confirmed that the issue was caused by the physical signal connections rather than the animation logic.

---

### 7. Colour System

**Status:** Done

The project defines several RGB colours:

- Red
- Green
- Blue
- Cyan
- Magenta
- Yellow
- White

The colours are converted using the display library's `color565()` function.

These colours are used by the display animations and text.

---

### 8. LUMEN Letter-by-Letter Intro

**Status:** Done

A letter-by-letter introduction was implemented for:

LUMEN

Each letter appears separately on the display.

The current colour sequence is:

- L → Red
- U → Yellow
- M → Green
- E → Cyan
- N → Magenta

The letters are displayed using the `showLetter()` function.

The display is cleared before each letter is shown.

---

### 9. LUMEN Scrolling Text

**Status:** Done

A normal scrolling animation was implemented for:

LUMEN

The text starts completely outside the right side of the display and moves across the screen toward the left.

The scrolling animation continuously changes the X-coordinate of the text.

The current scrolling delay is 35 ms per frame.

The text is displayed in white during the normal scrolling animation.

---

### 10. Final Display Sequence

**Status:** Working

The current main loop runs the following sequence:

L
↓
U
↓
M
↓
E
↓
N
↓
LUMEN scrolling across the display
↓
Repeat

The complete sequence continuously repeats.

---

## Current Project Status

**Overall Status: Working**

The LUMEN LED matrix is currently capable of:

- Driving a 64×32 RGB LED matrix
- Using the HUB75 interface
- Handling a 1/8 scan panel
- Correctly mapping the 8S panel coordinates
- Displaying multiple colours
- Displaying individual letters
- Showing the LUMEN introduction
- Smoothly scrolling LUMEN
- Repeating the complete display sequence

---

## Current Hardware

| Component | Status |
|---|---|
| ESP32 | Working |
| 64×32 RGB LED Matrix | Working |
| HUB75 Interface | Working |
| 1/8 Scan Mapping | Working |
| Power Supply | Working |
| HUB75 Wiring | Corrected and Working |
| SMD2525 LEDs | Identified |
| SM16208SF Driver ICs | Identified |

---

## Current Software

The project uses:

- Arduino IDE
- ESP32 board support
- `ESP32-HUB75-MatrixPanel-I2S-DMA`
- `ESP32-VirtualMatrixPanel-I2S-DMA`

The main source code is stored in:

`src/`

---

## Important Software Configuration

### Display Size

`PANEL_WIDTH = 64`

`PANEL_HEIGHT = 32`

### RGB Pins

`R1 = GPIO 25`

`G1 = GPIO 26`

`B1 = GPIO 27`

`R2 = GPIO 14`

`G2 = GPIO 12`

`B2 = GPIO 13`

### Row Address Pins

`A = GPIO 23`

`B = GPIO 19`

`C = GPIO 5`

`D = Not used`

`E = Not used`

### Control Pins

`LAT = GPIO 4`

`OE = GPIO 15`

`CLK = GPIO 16`

### Brightness

Brightness is initially set to 10 using the library's 8-bit brightness control.

### Clock Phase

Clock phase is configured as `false`.

---

## Project Milestones

- [x] Project repository created
- [x] LED matrix obtained
- [x] LED matrix specifications identified
- [x] ESP32 selected as controller
- [x] HUB75 interface identified
- [x] HUB75 wiring completed
- [x] Initial display test completed
- [x] Full-screen colour testing completed
- [x] 1/8 scan behaviour investigated
- [x] Custom 1/8 scan coordinate mapping implemented
- [x] Wiring issue diagnosed
- [x] Wiring corrected
- [x] Stable display output achieved
- [x] Multiple colours implemented
- [x] LUMEN letter-by-letter intro implemented
- [x] LUMEN scrolling implemented
- [x] Complete intro + scrolling sequence implemented

---

## Debugging Lessons

### Wiring Matters

The initial corrupted display output was not caused by the animation code.

Incorrect physical connections can produce:

- Random-looking pixels
- Blocks
- Incorrect colours
- Misplaced pixels
- Unstable display output

The HUB75 wiring should therefore be checked before modifying the display-mapping code.

### Scan Configuration Matters

A 1/8 scan panel cannot necessarily be treated like a standard full-scan display.

The project required a custom coordinate-mapping approach to correctly address the panel.

### Common Ground

The ESP32 and LED matrix must share a common ground for the HUB75 signals to work correctly.

---

## Repository Structure

LUMEN/
├── .git/
├── documentation/
│   ├── progress
│   └── notes
├── hardware/
│   ├── components
│   └── wiring
├── media/
│   ├── photos/
│   └── videos/
├── src/
└── README.md

---

## Next Possible Improvements

- Add more LED animations
- Add more scrolling messages
- Add custom fonts
- Add animated logos
- Add brightness controls
- Add colour effects
- Add transition effects
- Add interactive input
- Add wireless control
- Add additional display modes
- Add a physical enclosure for the matrix
- Add build photographs and videos to the documentation

---

## Current Summary

The LUMEN project has progressed from basic hardware identification and wiring to a functioning ESP32-controlled RGB LED matrix display.

The major technical challenges encountered so far were the HUB75 wiring and 1/8 scan coordinate mapping.

After resolving these issues, the panel successfully displays colours, individual letters, and scrolling text.

The current working display repeatedly shows a coloured letter-by-letter LUMEN introduction followed by a smooth white LUMEN scrolling animation.
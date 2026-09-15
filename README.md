# LUMEN — RGB LED Matrix Display

A programmable **64×32 RGB LED matrix display** built using an **ESP32** and a **HUB75 interface**.

LUMEN is an Electronics Society project exploring RGB LED matrices, HUB75 communication, 1/8 scan displays, ESP32 control, display mapping, and LED animations.

---

## Demo

The current display sequence:

**L → U → M → E → N**

followed by:

**LUMEN** scrolling across the display.

The letter-by-letter introduction uses different colours, followed by a white scrolling animation.

---

## Features

- 64×32 RGB LED matrix
- ESP32 controlled
- HUB75 interface
- 1/8 scan (8S)
- Custom 1/8 scan coordinate mapping
- RGB colour control
- Letter-by-letter LUMEN intro
- Smooth scrolling text
- Documented hardware and wiring
- Arduino source code

---

## Hardware

| Component | Details |
|---|---|
| Controller | ESP32 |
| Display | 64×32 RGB LED Matrix |
| Interface | HUB75 |
| Scan | 1/8 (8S) |
| LEDs | SMD2525 |
| Driver IC | SM16208SF |

Detailed hardware information:

**[Hardware Components](hardware/components)**

**[Wiring](hardware/wiring)**

---

## Software

Built using:

- Arduino IDE
- ESP32 board support
- ESP32-HUB75-MatrixPanel-I2S-DMA
- ESP32-VirtualMatrixPanel-I2S-DMA

Source code:

**[View Source Code](src/)**

---

## Project Progress

The project progressed from initial hardware testing and HUB75 wiring to a working 1/8-scan RGB matrix display.

Major milestones include:

- [x] ESP32 setup
- [x] Matrix panel setup
- [x] HUB75 connection
- [x] Initial display testing
- [x] 1/8 scan mapping
- [x] Wiring debugging
- [x] LUMEN intro
- [x] LUMEN scrolling

Detailed timeline:

**[Project Progress](documentation/progress)**

---

## Documentation

### Hardware

- **[Components](hardware/components)** — Hardware used in the project
- **[Wiring](hardware/wiring)** — Complete ESP32 → HUB75 wiring

### Documentation

- **[Progress](documentation/progress)** — Development timeline
- **[Notes](documentation/notes)** — Technical concepts, learning, and debugging

### Media

Photos and videos from the build are available in:

`media/`

---

## Repository Structure

LUMEN/
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

## Current Status

**Working**

The display can currently show the LUMEN introduction and smoothly scroll the word **LUMEN** across the 64×32 RGB matrix.

---

## Future Plans

- More animations
- Custom fonts
- Animated logos
- Additional display modes
- Wireless control
- Physical enclosure
- More interactive features

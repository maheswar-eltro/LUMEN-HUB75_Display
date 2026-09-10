# LUMEN RGB LED Matrix Display

An ESP32-controlled RGB LED matrix display built for showcasing electronics, embedded systems, and LED control.

## Project Overview

This project uses an ESP32 to control a P5 RGB LED matrix through the HUB75 interface.

The goal is to build a programmable display capable of showing:

- Scrolling text
- RGB animations
- LUMEN branding
- Custom messages
- Future interactive features

## Hardware

### LED Matrix
- P5 RGB LED Matrix
- Resolution: 64 × 32 pixels
- Pixel pitch: 5 mm
- HUB75 interface
- Outdoor / weatherproof panel
- PCB: P5_6432-8S-V2.1
- LED driver: SM16-series

### Controller
- ESP32

### Planned Additions
- 5V power supply
- RGB LED strip for the display frame
- Custom enclosure/frame

## System Architecture

```text
ESP32
  │
  │ HUB75
  ▼
P5 RGB LED Matrix
64 × 32 pixels
  ▲
  │
  │ 5V Power
  │
5V Power Supply
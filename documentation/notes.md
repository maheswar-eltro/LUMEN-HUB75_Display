# LUMEN — Technical Notes

## 1. ESP32 and HUB75

The ESP32 is used as the main controller for the LED matrix.

The LED matrix communicates with the ESP32 through the **HUB75 interface**.

HUB75 carries:

- RGB pixel data
- Row-address signals
- Clock signal
- Latch signal
- Output Enable signal
- Ground

The ESP32 does not directly power the LED matrix. The panel requires its own suitable power source, while the ESP32 and panel share a common ground.

---

## 2. 64×32 RGB Matrix

The panel has:

- Width = 64 pixels
- Height = 32 pixels

Therefore:

`64 × 32 = 2048 pixels`

Since the panel is an RGB display, each pixel contains three colour channels:

- Red
- Green
- Blue

The panel uses an **8S / 1/8 scan** architecture.

---

## 3. Understanding 1/8 Scan

A 1/8 scan panel does not drive all 32 rows independently at the same time.

Instead, the panel uses multiplexing, where a portion of the rows is addressed at a time.

For a 64×32 panel:

`64 × 32 = 2048 LEDs/pixels`

With 1/8 scanning:

`2048 ÷ 8 = 256`

So approximately **256 pixels are electrically addressed during each scan section**, while the display rapidly cycles through the different sections.

The rapid scanning makes the complete display appear continuously illuminated to the human eye.

---

## 4. HUB75 RGB Data Lines

The panel has two sets of RGB data inputs:

### Upper RGB

- R1
- G1
- B1

### Lower RGB

- R2
- G2
- B2

The two RGB groups are used because the panel's multiplexed architecture processes two corresponding portions of the display.

Current ESP32 connections:

- R1 → GPIO 25
- G1 → GPIO 26
- B1 → GPIO 27
- R2 → GPIO 14
- G2 → GPIO 12
- B2 → GPIO 13

---

## 5. Row Addressing

The panel uses address lines to select which row section is currently being displayed.

Current connections:

- A → GPIO 23
- B → GPIO 19
- C → GPIO 5

D and E are not used in the current configuration.

The software therefore defines:

`D_PIN = -1`

`E_PIN = -1`

---

## 6. HUB75 Control Signals

### CLK — Clock

CLK controls the timing of pixel data being shifted into the display.

Current connection:

`CLK → GPIO 16`

### LAT — Latch

LAT transfers the shifted pixel data into the display's output registers.

Current connection:

`LAT → GPIO 4`

### OE — Output Enable

OE controls whether the display output is enabled.

Current connection:

`OE → GPIO 15`

These signals work together with the RGB and row-address signals to refresh the panel.

---

## 7. Current GPIO Map

| Signal | ESP32 GPIO |
|---|---:|
| R1 | 25 |
| G1 | 26 |
| B1 | 27 |
| R2 | 14 |
| G2 | 12 |
| B2 | 13 |
| A | 23 |
| B | 19 |
| C | 5 |
| D | Not used |
| E | Not used |
| LAT | 4 |
| OE | 15 |
| CLK | 16 |

---

## 8. Driver IC

The driver IC identified on the panel is:

`SM16208SF`

The driver circuitry handles the electrical driving of the LED pixels based on the data received through the panel interface.

The ESP32 therefore does not individually control every LED directly.

Instead, the ESP32 sends display data through HUB75, and the panel's driver circuitry handles the corresponding LED driving.

---

## 9. SMD2525 LEDs

The panel uses **SMD2525** LED packages.

SMD refers to:

**Surface-Mount Device**

The `2525` designation refers to the approximate package dimensions.

These LEDs are arranged across the matrix to form the RGB pixels.

---

## 10. Why a Special Mapping Was Needed

A normal graphics library can treat a display as a simple coordinate grid:

`(0,0) → top-left`

`(63,31) → bottom-right`

However, the physical arrangement of a 1/8 scan panel does not necessarily follow this simple logical order.

During development, incorrect mapping produced unexpected display patterns.

The project therefore uses:

`VirtualMatrixPanel`

together with a custom:

`EightScanPanel`

class.

The custom `getCoords()` function converts the logical coordinates into the physical coordinates required by the 1/8 scan panel.

---

## 11. VirtualMatrixPanel

The project includes:

`ESP32-VirtualMatrixPanel-I2S-DMA.h`

The `VirtualMatrixPanel` system allows the software to work with a logical display layout while handling the physical arrangement of the LED matrix.

The project extends it using:

`EightScanPanel`

This is specifically used for the current 1/8 scan configuration.

---

## 12. MatrixPanel_I2S_DMA

The project also uses:

`ESP32-HUB75-MatrixPanel-I2S-DMA.h`

The DMA-based HUB75 library handles the high-speed transfer of display data from the ESP32 to the LED matrix.

DMA stands for:

**Direct Memory Access**

This allows data to be transferred efficiently without requiring the processor to manually handle every individual transfer.

---

## 13. Display Initialization

The display is initialized using a HUB75 configuration containing:

- Panel width
- Panel height
- Number of panels
- GPIO pin configuration

The project uses:

`64 × 32`

as the logical display resolution.

The configuration also sets:

`config.clkphase = false`

The display is then started using:

`dma_display->begin()`

and cleared using:

`dma_display->clearScreen()`

---

## 14. Brightness

The project currently sets the display brightness using:

`setBrightness8(10)`

The value is an 8-bit brightness setting.

The current brightness is intentionally kept low during testing.

---

## 15. RGB565 Colour

The display library uses a 16-bit colour representation.

The project converts RGB values using:

`color565()`

For example:

`color565(255, 0, 0)`

creates red.

The project currently defines:

- Red
- Green
- Blue
- Cyan
- Magenta
- Yellow
- White

---

## 16. Letter Display

The project uses the display's text functions to draw individual characters.

The `showLetter()` function:

1. Clears the screen.
2. Sets the text size.
3. Sets the text colour.
4. Positions the cursor.
5. Prints the character.
6. Keeps it on screen for a short period.

This is used to create the LUMEN introduction.

---

## 17. LUMEN Intro

The introduction displays the letters individually:

`L → U → M → E → N`

Each letter uses a different colour:

- L → Red
- U → Yellow
- M → Green
- E → Cyan
- N → Magenta

After the letters have been displayed individually, the program moves to the scrolling animation.

---

## 18. Scrolling Text

The scrolling animation works by repeatedly changing the X-coordinate of the text.

The text starts outside the right side of the display.

The X-coordinate is gradually decreased:

`x--`

This makes the text move from right to left.

The display is cleared and redrawn for each position.

The current animation uses:

`delay(35)`

between frames.

The scrolling text is:

`LUMEN`

and is displayed in white.

---

## 19. Text Size and Position

The scrolling text currently uses:

`setTextSize(3)`

The vertical position is:

`y = 6`

The letter-by-letter intro also uses text size 3.

The cursor position for the individual letters is currently:

`x = 26`

`y = 6`

---

## 20. Important Debugging Lesson

One of the main problems encountered during development was corrupted or box-like output.

The first assumption could have been that the coordinate-mapping code was incorrect.

However, the actual problem was the **physical wiring**.

After the wiring was corrected, the display worked correctly.

This demonstrated an important hardware debugging principle:

**Do not assume a software problem when the hardware connections have not been verified.**

---

## 21. Debugging Order

For future problems, a useful debugging order is:

1. Check power.
2. Check common ground.
3. Check HUB75 connector orientation.
4. Check RGB data connections.
5. Check row-address connections.
6. Check CLK.
7. Check LAT.
8. Check OE.
9. Check panel scan configuration.
10. Check coordinate mapping.
11. Check animation code.

This prevents unnecessary changes to working software when the actual problem is wiring.

---

## 22. Power and Ground

The LED matrix requires significantly more power than the ESP32 can provide.

The matrix therefore uses an external power source.

The ESP32 should not be expected to power the entire LED panel.

The ESP32 and LED matrix must have a common ground so that the HUB75 signal voltages have the same reference.

Basic arrangement:

`Power Supply → LED Matrix`

`ESP32 → HUB75 signals`

`ESP32 GND ↔ Matrix GND`

---

## 23. Current Software Structure

The main program is divided into several logical sections:

1. Pin definitions
2. Panel configuration
3. 1/8 scan coordinate mapping
4. Display initialization
5. Colour definitions
6. Letter display function
7. LUMEN introduction
8. LUMEN scrolling
9. Main loop

This makes the code easier to modify when adding new animations or display modes.

---

## 24. Current Display Flow

The current program follows this sequence:

`Setup`

↓

`Initialize HUB75 display`

↓

`Initialize 1/8 scan mapping`

↓

`Create colours`

↓

`L`

↓

`U`

↓

`M`

↓

`E`

↓

`N`

↓

`Scroll LUMEN`

↓

`Repeat`

---

## 25. Main Things Learned

Through the LUMEN project, the main concepts explored so far are:

- ESP32 GPIO control
- HUB75 display interfaces
- RGB LED matrices
- 1/8 scan architecture
- Multiplexed LED displays
- LED driver ICs
- DMA-based display driving
- Coordinate mapping
- RGB565 colours
- Text rendering
- Scrolling animations
- Hardware debugging
- Power and common-ground requirements
- Arduino libraries
- Structuring an electronics project repository

---

## 26. Key Takeaway

The ESP32 does not simply turn individual LEDs on and off one at a time.

Instead, the ESP32 continuously sends timed RGB data and row-address information through the HUB75 interface.

The matrix driver circuitry and scanning system then use this data to refresh the display rapidly.

The combination of:

`ESP32`

+

`HUB75`

+

`SM16208SF driver circuitry`

+

`1/8 scan architecture`

+

`DMA-based display library`

allows the 64×32 matrix to function as a programmable RGB display.

---

## 27. Current Configuration Reference

```text
Panel:
64 × 32 RGB
1/8 Scan (8S)
HUB75
SMD2525 LEDs
SM16208SF driver IC

Controller:
ESP32

RGB:
R1 → GPIO 25
G1 → GPIO 26
B1 → GPIO 27
R2 → GPIO 14
G2 → GPIO 12
B2 → GPIO 13

Address:
A → GPIO 23
B → GPIO 19
C → GPIO 5
D → Not used
E → Not used

Control:
LAT → GPIO 4
OE  → GPIO 15
CLK → GPIO 16

Current display:
L → U → M → E → N
        ↓
    LUMEN scroll
        ↓
      Repeat
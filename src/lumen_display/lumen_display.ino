#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"

// =====================================================
// PINS
// =====================================================

#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27

#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13

#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN -1
#define E_PIN -1

#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32


// =====================================================
// 1/8 SCAN PANEL MAPPING
// =====================================================

class EightScanPanel : public VirtualMatrixPanel {
public:
    using VirtualMatrixPanel::VirtualMatrixPanel;

protected:
    VirtualCoords getCoords(int16_t x, int16_t y);
};

inline VirtualCoords EightScanPanel::getCoords(int16_t x, int16_t y)
{
    coords = VirtualMatrixPanel::getCoords(x, y);

    if (coords.x < 0 || coords.y < 0)
        return coords;

    if ((coords.y & 8) == 0)
    {
        coords.x += ((coords.x / panelResX) + 1) * panelResX;
    }
    else
    {
        coords.x += (coords.x / panelResX) * panelResX;
    }

    coords.y = (coords.y >> 4) * 8 + (coords.y & 0b00000111);

    return coords;
}


// =====================================================
// DISPLAY
// =====================================================

MatrixPanel_I2S_DMA *dma_display = nullptr;
EightScanPanel *display = nullptr;


// =====================================================
// COLORS
// =====================================================

uint16_t red;
uint16_t green;
uint16_t blue;
uint16_t cyan;
uint16_t magenta;
uint16_t yellow;
uint16_t white;


// =====================================================
// SETUP
// =====================================================

void setup()
{
    HUB75_I2S_CFG::i2s_pins pins = {
        R1_PIN,
        G1_PIN,
        B1_PIN,

        R2_PIN,
        G2_PIN,
        B2_PIN,

        A_PIN,
        B_PIN,
        C_PIN,
        D_PIN,
        E_PIN,

        LAT_PIN,
        OE_PIN,
        CLK_PIN
    };

    HUB75_I2S_CFG config(
        PANEL_WIDTH * 2,
        PANEL_HEIGHT / 2,
        1,
        pins
    );

    config.clkphase = false;

    dma_display = new MatrixPanel_I2S_DMA(config);

    dma_display->begin();
    dma_display->setBrightness8(10);
    dma_display->clearScreen();

    display = new EightScanPanel(
        *dma_display,
        1,
        1,
        PANEL_WIDTH,
        PANEL_HEIGHT
    );

    display->setTextWrap(false);

    // Colors
    red     = display->color565(255, 0, 0);
    green   = display->color565(0, 255, 0);
    blue    = display->color565(0, 0, 255);
    cyan    = display->color565(0, 255, 255);
    magenta = display->color565(255, 0, 255);
    yellow  = display->color565(255, 255, 0);
    white   = display->color565(255, 255, 255);
}


// =====================================================
// SHOW ONE LETTER
// =====================================================

void showLetter(char letter, uint16_t color)
{
    display->fillScreen(0);

    display->setTextSize(3);
    display->setTextColor(color);

    display->setCursor(26, 6);
    display->print(letter);

    delay(500);

    display->setTextColor(color);
    display->setCursor(26, 6);
    display->print(letter);

    delay(450);
}


// =====================================================
// LUMEN INTRO
// =====================================================

void lumenIntro()
{
    showLetter('L', red);
    showLetter('U', yellow);
    showLetter('M', green);
    showLetter('E', cyan);
    showLetter('N', magenta);

    delay(400);
}


// =====================================================
// NORMAL LUMEN SCROLL
// =====================================================

void scrollLumen()
{
    display->setTextSize(3);
    display->setTextWrap(false);
    display->setTextColor(white);

    int y = 6;

    // Start completely off the right side
    for (int x = PANEL_WIDTH; x > -90; x--)
    {
        display->fillScreen(0);

        display->setCursor(x, y);
        display->print("LUMEN");

        delay(35);
    }
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
    // Letter-by-letter intro
    lumenIntro();

    // Normal smooth LUMEN scroll
    scrollLumen();

    delay(500);
}
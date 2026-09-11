#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"

/*
===============================
    ESP32 → HUB75
===============================
*/

#define R1_PIN   25
#define G1_PIN   26
#define B1_PIN   27

#define R2_PIN   14
#define G2_PIN   12
#define B2_PIN   13

#define A_PIN    23
#define B_PIN    19
#define C_PIN    5

// 1/8 scan test:
// D and E are intentionally unused.
#define D_PIN    -1
#define E_PIN    -1

#define LAT_PIN  4
#define OE_PIN   15
#define CLK_PIN  16


/*
===============================
    PANEL
===============================
*/

#define PANEL_WIDTH   64
#define PANEL_HEIGHT  32


MatrixPanel_I2S_DMA *dma_display = nullptr;


/* 
===============================
    1/8 SCAN MAPPING
===============================
*/

class EightScanPanel : public VirtualMatrixPanel
{
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

    // 1st / 3rd blocks of 8 rows
    if ((coords.y & 8) == 0)
    {
        coords.x += ((coords.x / panelResX) + 1) * panelResX;
    }
    else
    {
        // 2nd / 4th blocks of 8 rows
        coords.x += (coords.x / panelResX) * panelResX;
    }

    coords.y = (coords.y >> 4) * 8 + (coords.y & 0b00000111);

    return coords;
}


EightScanPanel *display = nullptr;

/*
===============================
    SETUP
===============================
*/

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("==============================");
    Serial.println("P5 64x32 1/8 SCAN TEST");
    Serial.println("==============================");


    HUB75_I2S_CFG::i2s_pins pins =
    {
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


    // The DMA engine is internally configured as 128x16.
    // The virtual panel maps that back to our physical 64x32 panel.

    HUB75_I2S_CFG config(
        PANEL_WIDTH * 2,
        PANEL_HEIGHT / 2,
        1,
        pins
    );


    // Try this first.
    config.clkphase = false;


    dma_display = new MatrixPanel_I2S_DMA(config);


    dma_display->setBrightness8(40);


    if (!dma_display->begin())
    {
        Serial.println("DMA INIT FAILED");
        while (true)
        {
            delay(1000);
        }
    }


    dma_display->clearScreen();


    display = new EightScanPanel(
        *dma_display,
        1,
        1,
        PANEL_WIDTH,
        PANEL_HEIGHT
    );


    Serial.println("DISPLAY INITIALIZED");
}

/*
===============================
    TEST
===============================
*/

void loop()
{
    for (int x = PANEL_WIDTH; x > -95; x--)
    {
        display->fillScreen(display->color565(0, 0, 0));

        display->setTextSize(2);
        display->setTextWrap(false);
        display->setTextColor(
            display->color565(255, 0, 0)
        );

        display->setCursor(x, 10);
        display->print("Hello!");

        delay(50);
    }
}


#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

// =====================================================
// YOUR WORKING HUB75 PIN CONFIGURATION
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
// YOUR CUSTOM 1/8-SCAN MAPPING
// =====================================================

class EightScanPanel : public VirtualMatrixPanel {
public:
    using VirtualMatrixPanel::VirtualMatrixPanel;

protected:
    VirtualCoords getCoords(int16_t x, int16_t y);
};

inline VirtualCoords EightScanPanel::getCoords(
    int16_t x, int16_t y
) {
    coords = VirtualMatrixPanel::getCoords(x, y);

    if (coords.x < 0 || coords.y < 0) {
        return coords;
    }

    if ((coords.y & 8) == 0) {
        coords.x +=
            ((coords.x / panelResX) + 1) * panelResX;
    } else {
        coords.x +=
            (coords.x / panelResX) * panelResX;
    }

    coords.y =
        (coords.y >> 4) * 8 +
        (coords.y & 0b00000111);

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

uint16_t BLACK;
uint16_t WHITE;
uint16_t DIM_BLUE;
uint16_t BLUE;
uint16_t CYAN;
uint16_t PURPLE;
uint16_t YELLOW;
uint16_t ORANGE;
uint16_t PINK;

// =====================================================
// TIMING
// =====================================================

unsigned long lastFrame = 0;
unsigned long sceneStart = 0;

const unsigned long FRAME_DELAY = 35;

// =====================================================
// SCENES
// =====================================================

enum Scene {
    BULB_RISE,
    BULB_FLICKER,
    CAMERA_LAUNCH,
    GALAXY_SCROLL,
    END_HOLD
};

Scene scene = BULB_RISE;

// =====================================================
// BULB ANIMATION
// =====================================================

int bulbY = 36;

const int BULB_X = 32;

bool bulbLit = false;

int flickerCount = 0;
unsigned long lastFlicker = 0;

// =====================================================
// CAMERA / STARFIELD
// =====================================================

const int STAR_COUNT = 42;

struct Star {
    float x;
    float y;
    float vx;
    float vy;
    uint8_t brightness;
};

Star stars[STAR_COUNT];

float launchSpeed = 0;

// =====================================================
// GALAXY TEXT SCROLL
// =====================================================

int textX = PANEL_WIDTH;

const int TEXT_Y = 11;

unsigned long lastTextMove = 0;

const unsigned long TEXT_SCROLL_DELAY = 160;

// =====================================================
// STAR FUNCTIONS
// =====================================================

void createStar(int i, bool anywhere = true) {
    stars[i].x = random(0, PANEL_WIDTH);

    if (anywhere) {
        stars[i].y = random(0, PANEL_HEIGHT);
    } else {
        stars[i].y = random(-10, 0);
    }

    stars[i].vx = 0;
    stars[i].vy = 0;

    stars[i].brightness = random(1, 4);
}

void initStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        createStar(i);
    }
}

uint16_t starColor(uint8_t b) {
    if (b == 1) return DIM_BLUE;
    if (b == 2) return CYAN;
    return WHITE;
}

void drawStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        display->drawPixel(
            (int)stars[i].x,
            (int)stars[i].y,
            starColor(stars[i].brightness)
        );
    }
}

// =====================================================
// BULB DRAWING
// =====================================================

// Draws a simple pixel-art light bulb.
// Centered at x, with the top at y.

void drawBulb(int x, int y, bool glow) {
    uint16_t bulbColor = glow ? YELLOW : ORANGE;

    // Outer bulb shape
    display->drawLine(x - 3, y, x + 3, y, bulbColor);
    display->drawLine(x - 4, y + 1, x + 4, y + 1, bulbColor);
    display->drawLine(x - 4, y + 2, x + 4, y + 2, bulbColor);
    display->drawPixel(x - 3, y + 3, bulbColor);
    display->drawPixel(x + 3, y + 3, bulbColor);

    // Neck
    display->drawLine(x - 2, y + 4, x + 2, y + 4, ORANGE);
    display->drawLine(x - 2, y + 5, x + 2, y + 5, ORANGE);

    // Base
    display->drawLine(x - 2, y + 6, x + 2, y + 6, WHITE);
    display->drawLine(x - 2, y + 7, x + 2, y + 7, WHITE);
    display->drawLine(x - 1, y + 8, x + 1, y + 8, WHITE);

    if (glow) {
        // Rays
        display->drawPixel(x, y - 2, YELLOW);
        display->drawPixel(x - 6, y + 1, YELLOW);
        display->drawPixel(x + 6, y + 1, YELLOW);
        display->drawPixel(x - 5, y - 3, ORANGE);
        display->drawPixel(x + 5, y - 3, ORANGE);
    }
}

// =====================================================
// SCENE 1: BULB RISES FROM BOTTOM
// =====================================================

void animateBulbRise() {
    unsigned long elapsed = millis() - sceneStart;

    // Smooth-ish upward movement.
    bulbY = 36 - (elapsed / 70);

    if (bulbY < 10) {
        bulbY = 10;
    }

    drawBulb(BULB_X, bulbY, false);

    if (elapsed > 1800) {
        scene = BULB_FLICKER;
        sceneStart = millis();

        flickerCount = 0;
        lastFlicker = 0;
    }
}

// =====================================================
// SCENE 2: BULB FLICKERS
// =====================================================

void animateBulbFlicker() {
    unsigned long elapsed = millis() - sceneStart;

    if (millis() - lastFlicker > 180) {
        lastFlicker = millis();

        bulbLit = !bulbLit;
        flickerCount++;
    }

    drawBulb(BULB_X, 10, bulbLit);

    if (elapsed > 1500) {
        bulbLit = true;

        scene = CAMERA_LAUNCH;
        sceneStart = millis();

        launchSpeed = 0;

        initStars();
    }
}

// =====================================================
// SCENE 3: FAST CAMERA LAUNCH
// =====================================================

void animateCameraLaunch() {
    unsigned long elapsed = millis() - sceneStart;

    // Bulb flashes brightly at launch.
    if (elapsed < 250) {
        drawBulb(BULB_X, 10, true);
    }

    // Accelerate star movement.
    launchSpeed += 0.55;

    if (launchSpeed > 12) {
        launchSpeed = 12;
    }

    for (int i = 0; i < STAR_COUNT; i++) {
        // Camera moves upward-right through space,
        // making stars streak downward-left.
        stars[i].x -= launchSpeed * 0.65;
        stars[i].y += launchSpeed;

        if (stars[i].x < 0 ||
            stars[i].y >= PANEL_HEIGHT) {
            createStar(i, false);
            stars[i].x = random(0, PANEL_WIDTH);
        }
    }

    // Streaks create the fast-pan effect.
    for (int i = 0; i < STAR_COUNT; i++) {
        int x = (int)stars[i].x;
        int y = (int)stars[i].y;

        display->drawLine(
            x, y,
            x + 2, y - 3,
            starColor(stars[i].brightness)
        );
    }

    if (elapsed > 1800) {
        scene = GALAXY_SCROLL;
        sceneStart = millis();

        textX = PANEL_WIDTH;

        // Reset stars for calmer side-scrolling.
        initStars();
    }
}

// =====================================================
// SCENE 4: GALAXY SIDE-SCROLL + LUMEN
// =====================================================

void animateGalaxyScroll() {
    // Slow side-scrolling stars.
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x -= 0.45;

        if (stars[i].x < 0) {
            stars[i].x = PANEL_WIDTH - 1;
            stars[i].y = random(0, PANEL_HEIGHT);
            stars[i].brightness = random(1, 4);
        }
    }

    drawStars();

    // Small purple/cyan galaxy swirl.
    display->drawPixel(8, 5, PURPLE);
    display->drawPixel(9, 6, PURPLE);
    display->drawPixel(10, 7, CYAN);
    display->drawPixel(11, 8, PURPLE);
    display->drawPixel(12, 7, CYAN);
    display->drawPixel(13, 6, PURPLE);

    // LUMEN scrolls across the galaxy.
    display->setTextSize(2);
    display->setTextColor(PINK);
    display->setCursor(textX, TEXT_Y);
    display->print("LUMEN");

    if (millis() - lastTextMove > TEXT_SCROLL_DELAY) {
        lastTextMove = millis();
        textX--;
    }

    // Text width is approximately 60 pixels at size 2.
    if (textX < -60) {
        scene = END_HOLD;
        sceneStart = millis();
    }
}

// =====================================================
// SCENE 5: FINAL LOGO HOLD
// =====================================================

void animateEndHold() {
    drawStars();

    display->setTextSize(2);
    display->setTextColor(YELLOW);
    display->setCursor(4, 10);
    display->print("LUMEN");

    // Decorative sparkles.
    display->drawPixel(2, 5, CYAN);
    display->drawPixel(58, 6, WHITE);
    display->drawPixel(55, 25, PURPLE);
    display->drawPixel(8, 26, CYAN);

    if (millis() - sceneStart > 3000) {
        scene = BULB_RISE;
        sceneStart = millis();

        bulbY = 36;
        bulbLit = false;

        initStars();
    }
}

// =====================================================
// SETUP
// =====================================================

void setup() {
    Serial.begin(115200);
    randomSeed(micros());

    HUB75_I2S_CFG::i2s_pins pins = {
        R1_PIN, G1_PIN, B1_PIN,
        R2_PIN, G2_PIN, B2_PIN,
        A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
        LAT_PIN, OE_PIN, CLK_PIN
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
    dma_display->setBrightness8(40);
    dma_display->clearScreen();

    display = new EightScanPanel(
        *dma_display,
        1, 1,
        PANEL_WIDTH,
        PANEL_HEIGHT
    );

    display->setTextWrap(false);

    BLACK = display->color565(0, 0, 0);
    WHITE = display->color565(255, 255, 255);
    DIM_BLUE = display->color565(15, 20, 70);
    BLUE = display->color565(0, 50, 180);
    CYAN = display->color565(0, 220, 255);
    PURPLE = display->color565(120, 0, 255);
    YELLOW = display->color565(255, 220, 0);
    ORANGE = display->color565(255, 100, 0);
    PINK = display->color565(255, 0, 150);

    initStars();

    sceneStart = millis();

    Serial.println("LUMEN Space Journey Started");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
    if (millis() - lastFrame < FRAME_DELAY) {
        return;
    }

    lastFrame = millis();

    display->fillScreen(BLACK);

    switch (scene) {
        case BULB_RISE:
            animateBulbRise();
            break;

        case BULB_FLICKER:
            animateBulbFlicker();
            break;

        case CAMERA_LAUNCH:
            animateCameraLaunch();
            break;

        case GALAXY_SCROLL:
            animateGalaxyScroll();
            break;

        case END_HOLD:
            animateEndHold();
            break;
    }
}
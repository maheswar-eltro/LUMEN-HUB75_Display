
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#include <math.h>

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
// DISPLAY OBJECTS
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
uint16_t PINK;
uint16_t PURPLE;
uint16_t YELLOW;
uint16_t ORANGE;
uint16_t RED;
uint16_t GREEN;

// =====================================================
// FRAME TIMING
// =====================================================

const unsigned long FRAME_DELAY = 35;

unsigned long lastFrame = 0;
unsigned long sceneStart = 0;

// =====================================================
// SCENES
// =====================================================

enum Scene {
    BULB_RISE,
    BULB_FLICKER,
    CAMERA_LAUNCH,
    SHIP_FLYBY,
    LUMEN_REVEAL,
    PACMAN_EATS,
    BLACK_HOLD
};

Scene scene = BULB_RISE;

// =====================================================
// BULB
// =====================================================

const int BULB_X = 32;
int bulbY = 36;

bool bulbLit = false;

// =====================================================
// STARFIELD
// =====================================================

const int STAR_COUNT = 42;

struct Star {
    float x;
    float y;
    float speed;
    uint8_t brightness;
};

Star stars[STAR_COUNT];

// =====================================================
// CAMERA LAUNCH
// =====================================================

float launchSpeed = 0;

// =====================================================
// RETRO SPACESHIP
// =====================================================

float shipX = -14;
int shipY = 15;

unsigned long lastShipMove = 0;

const unsigned long SHIP_MOVE_DELAY = 55;

// =====================================================
// LUMEN TEXT
// =====================================================

int textX = PANEL_WIDTH;

const int TEXT_Y = 10;
const int TEXT_WIDTH = 60;

unsigned long lastTextMove = 0;

const unsigned long TEXT_SCROLL_DELAY = 140;

// =====================================================
// PAC-MAN
// =====================================================

float pacX = 72;

int pacY = 16;

bool pacMouthOpen = true;

unsigned long lastPacMove = 0;
unsigned long lastMouthToggle = 0;

const unsigned long PAC_MOVE_DELAY = 65;
const unsigned long PAC_MOUTH_DELAY = 130;

// =====================================================
// STAR FUNCTIONS
// =====================================================

void createStar(int i, bool randomY = true) {
    stars[i].x = random(0, PANEL_WIDTH);

    if (randomY) {
        stars[i].y = random(0, PANEL_HEIGHT);
    } else {
        stars[i].y = random(-8, 0);
    }

    stars[i].speed = random(10, 40) / 100.0;
    stars[i].brightness = random(1, 4);
}

void initStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        createStar(i);
    }
}

uint16_t getStarColor(uint8_t brightness) {
    if (brightness == 1) return DIM_BLUE;
    if (brightness == 2) return BLUE;
    return WHITE;
}

void drawStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        display->drawPixel(
            (int)stars[i].x,
            (int)stars[i].y,
            getStarColor(stars[i].brightness)
        );
    }
}

void updateSideStars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x -= stars[i].speed;

        if (stars[i].x < 0) {
            stars[i].x = PANEL_WIDTH - 1;
            stars[i].y = random(0, PANEL_HEIGHT);
            stars[i].brightness = random(1, 4);
        }
    }
}

// =====================================================
// BULB DRAWING
// =====================================================

// Pixel-art yellow light bulb.
// x = horizontal center; y = top of bulb.

void drawBulb(int x, int y, bool glow) {
    uint16_t bulbColor = glow ? YELLOW : ORANGE;

    // Glass
    display->drawLine(
        x - 3, y,
        x + 3, y,
        bulbColor
    );

    display->drawLine(
        x - 4, y + 1,
        x + 4, y + 1,
        bulbColor
    );

    display->drawLine(
        x - 4, y + 2,
        x + 4, y + 2,
        bulbColor
    );

    display->drawLine(
        x - 3, y + 3,
        x + 3, y + 3,
        bulbColor
    );

    // Neck
    display->drawLine(
        x - 2, y + 4,
        x + 2, y + 4,
        ORANGE
    );

    display->drawLine(
        x - 2, y + 5,
        x + 2, y + 5,
        ORANGE
    );

    // Metal base
    display->drawLine(
        x - 2, y + 6,
        x + 2, y + 6,
        WHITE
    );

    display->drawLine(
        x - 2, y + 7,
        x + 2, y + 7,
        WHITE
    );

    display->drawLine(
        x - 1, y + 8,
        x + 1, y + 8,
        WHITE
    );

    if (glow) {
        // Light rays
        display->drawPixel(x, y - 2, YELLOW);
        display->drawPixel(x - 6, y + 1, YELLOW);
        display->drawPixel(x + 6, y + 1, YELLOW);
        display->drawPixel(x - 5, y - 3, ORANGE);
        display->drawPixel(x + 5, y - 3, ORANGE);
    }
}

// =====================================================
// RETRO SPACESHIP DRAWING
// =====================================================

// Small 16-bit arcade-style spaceship pointing right.

void drawSpaceship(int x, int y) {
    // Engine flame
    display->drawLine(
        x - 5, y - 1,
        x - 8, y,
        ORANGE
    );

    display->drawLine(
        x - 5, y,
        x - 8, y + 1,
        YELLOW
    );

    // Main hull
    display->fillTriangle(
        x + 7, y,
        x - 4, y - 4,
        x - 4, y + 4,
        CYAN
    );

    // Cockpit
    display->drawPixel(
        x + 1, y,
        WHITE
    );

    // Upper wing
    display->drawLine(
        x - 3, y - 3,
        x - 6, y - 6,
        PURPLE
    );

    display->drawLine(
        x - 6, y - 6,
        x + 1, y - 3,
        PURPLE
    );

    // Lower wing
    display->drawLine(
        x - 3, y + 3,
        x - 6, y + 6,
        PURPLE
    );

    display->drawLine(
        x - 6, y + 6,
        x + 1, y + 3,
        PURPLE
    );
}

// =====================================================
// PAC-MAN DRAWING
// =====================================================

// Draw Pac-Man facing LEFT.
// The black triangular mouth opens toward the left.

void drawPacman(int x, int y, bool mouthOpen) {
    const int R = 9;

    // Body
    display->fillCircle(
        x, y,
        R,
        YELLOW
    );

    if (mouthOpen) {
        // Cut a black wedge out of the left side.
        display->fillTriangle(
            x, y,
            x - R - 1, y - 4,
            x - R - 1, y + 4,
            BLACK
        );
    }

    // Tiny eye
    display->drawPixel(
        x, y - 3,
        BLACK
    );
}

// =====================================================
// CAMERA LAUNCH STAR STREAKS
// =====================================================

void updateLaunchStars() {
    launchSpeed += 0.55;

    if (launchSpeed > 12) {
        launchSpeed = 12;
    }

    for (int i = 0; i < STAR_COUNT; i++) {
        // Fast diagonal camera movement:
        // stars sweep down and diagonally.

        stars[i].x -= launchSpeed * 0.65;
        stars[i].y += launchSpeed;

        if (stars[i].x < 0 ||
            stars[i].y >= PANEL_HEIGHT) {
            createStar(i, false);
            stars[i].x = random(0, PANEL_WIDTH);
        }
    }
}

void drawLaunchStreaks() {
    for (int i = 0; i < STAR_COUNT; i++) {
        int x = (int)stars[i].x;
        int y = (int)stars[i].y;

        display->drawLine(
            x, y,
            x + 2, y - 3,
            getStarColor(stars[i].brightness)
        );
    }
}

// =====================================================
// SCENE 1: BULB RISES
// =====================================================

void animateBulbRise() {
    unsigned long elapsed = millis() - sceneStart;

    bulbY = 36 - (elapsed / 70);

    if (bulbY < 10) {
        bulbY = 10;
    }

    drawBulb(BULB_X, bulbY, false);

    if (elapsed > 1800) {
        scene = BULB_FLICKER;
        sceneStart = millis();
    }
}

// =====================================================
// SCENE 2: BULB FLICKERS
// =====================================================

void animateBulbFlicker() {
    unsigned long elapsed = millis() - sceneStart;

    if (elapsed < 1100) {
        bulbLit = ((elapsed / 120) % 2 == 0);
    } else {
        bulbLit = true;
    }

    drawBulb(BULB_X, 10, bulbLit);

    if (elapsed > 1600) {
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

    updateLaunchStars();
    drawLaunchStreaks();

    // Brief bulb flash as the camera launches.
    if (elapsed < 250) {
        drawBulb(BULB_X, 10, true);
    }

    if (elapsed > 1800) {
        scene = SHIP_FLYBY;
        sceneStart = millis();

        shipX = -14;
        shipY = 15;

        initStars();
    }
}

// =====================================================
// SCENE 4: RETRO SPACESHIP FLIES PAST
// =====================================================

void animateShipFlyby() {
    updateSideStars();
    drawStars();

    unsigned long elapsed = millis() - sceneStart;

    if (millis() - lastShipMove >= SHIP_MOVE_DELAY) {
        lastShipMove = millis();

        shipX += 2;

        // Small vertical wobble
        shipY = 15 + (int)(3 * sin(elapsed / 180.0));
    }

    drawSpaceship((int)shipX, shipY);

    if (shipX > PANEL_WIDTH + 12) {
        scene = LUMEN_REVEAL;
        sceneStart = millis();

        textX = PANEL_WIDTH;
        lastTextMove = millis();
    }
}

// =====================================================
// SCENE 5: LUMEN SCROLLS THROUGH SPACE
// =====================================================

void animateLumenReveal() {
    updateSideStars();
    drawStars();

    display->setTextSize(2);
    display->setTextColor(PINK);

    display->setCursor(textX, TEXT_Y);
    display->print("LUMEN");

    if (millis() - lastTextMove >= TEXT_SCROLL_DELAY) {
        lastTextMove = millis();

        textX--;
    }

    // Once LUMEN is centered, pause for a moment,
    // then bring Pac-Man in from the right.
    if (textX <= 4) {
        scene = PACMAN_EATS;
        sceneStart = millis();

        pacX = 72;
        pacY = 16;

        lastPacMove = millis();
        lastMouthToggle = millis();
    }
}

// =====================================================
// SCENE 6: PAC-MAN EATS LUMEN
// =====================================================

void animatePacmanEats() {
    // Draw the stars and word first.
    updateSideStars();
    drawStars();

    display->setTextSize(2);
    display->setTextColor(PINK);

    display->setCursor(4, TEXT_Y);
    display->print("LUMEN");

    // Move Pac-Man from right to left.
    if (millis() - lastPacMove >= PAC_MOVE_DELAY) {
        lastPacMove = millis();

        pacX -= 1.5;
    }

    // Alternate open/closed mouth.
    if (millis() - lastMouthToggle >= PAC_MOUTH_DELAY) {
        lastMouthToggle = millis();

        pacMouthOpen = !pacMouthOpen;
    }

    // Erase the part Pac-Man has already eaten.
    // Since Pac-Man travels left, everything to
    // his right becomes empty black space.
    int eraseFromX = (int)pacX + 5;

    if (eraseFromX < PANEL_WIDTH) {
        if (eraseFromX < 0) {
            eraseFromX = 0;
        }

        display->fillRect(
            eraseFromX,
            0,
            PANEL_WIDTH - eraseFromX,
            PANEL_HEIGHT,
            BLACK
        );
    }

    // Draw Pac-Man over the text.
    drawPacman(
        (int)pacX,
        pacY,
        pacMouthOpen
    );

    // Once Pac-Man exits, clear the entire screen.
    if (pacX < -8) {
        scene = BLACK_HOLD;
        sceneStart = millis();
    }
}

// =====================================================
// SCENE 7: EMPTY BLACK SCREEN
// =====================================================

void animateBlackHold() {
    // Intentionally draw nothing.
    // Main loop clears the screen every frame.

    if (millis() - sceneStart > 1800) {
        scene = BULB_RISE;
        sceneStart = millis();

        bulbY = 36;
        bulbLit = false;

        launchSpeed = 0;

        shipX = -14;
        pacX = 72;

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

    // Colors
    BLACK = display->color565(0, 0, 0);
    WHITE = display->color565(255, 255, 255);

    DIM_BLUE = display->color565(10, 15, 50);
    BLUE = display->color565(0, 45, 180);
    CYAN = display->color565(0, 210, 255);
    PINK = display->color565(255, 0, 255);
    PURPLE = display->color565(120, 0, 255);

    YELLOW = display->color565(255, 220, 0);
    ORANGE = display->color565(255, 100, 0);
    RED = display->color565(255, 0, 0);
    GREEN = display->color565(0, 255, 80);

    initStars();

    sceneStart = millis();

    Serial.println("LUMEN Retro Space Animation Started");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
    if (millis() - lastFrame < FRAME_DELAY) {
        return;
    }

    lastFrame = millis();

    // Every frame starts from a clean black screen.
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

        case SHIP_FLYBY:
            animateShipFlyby();
            break;

        case LUMEN_REVEAL:
            animateLumenReveal();
            break;

        case PACMAN_EATS:
            animatePacmanEats();
            break;

        case BLACK_HOLD:
            animateBlackHold();
            break;
    }
}
#include "display.h"

display::display() {
    backgroundColor = 0xC618;
    effectColor = 0x7BEF;
    menuColor = 0xFDA0;

    Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
        40, 41, 39, 42, 45, 48, 47, 21, 14, 5, 6, 7, 15, 16, 4, 8, 3, 46, 9, 1, 0, 210, 4, 43, 0, 12, 4, 12, 1, 16000000);
    gfx = new Arduino_RGB_Display(800, 480, rgbpanel, 0, true);
}

bool display::begin() {
    return gfx->begin();
}

void display::clearScreen() {
    gfx->fillScreen(backgroundColor);
}

void display::fillRect(int x, int y, int width, int height, uint16_t color) {
    gfx->fillRect(x, y, width, height, color);
}

void display::printText(int x, int y, const String& text, uint16_t color) {
    gfx->setCursor(x, y);
    gfx->setTextColor(color);
    gfx->setTextSize(2);
    gfx->print(text);
}

void display::showMenu(int x, int y) {
    fillRect(x + 120, y, 100, 50, menuColor);
    printText(x + 135, y + 15, "Delete", 0x0000);
}

void display::hideMenu(int x, int y) {
    fillRect(x + 120, y, 100, 50, backgroundColor);
}

// Implement the getWidth() and getHeight() methods
int display::getWidth() {
    return gfx->width();
}

int display::getHeight() {
    return gfx->height();
}

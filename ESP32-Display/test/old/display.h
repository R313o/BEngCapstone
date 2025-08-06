#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino_GFX_Library.h>

class display {
public:
    display();
    bool begin();
    void clearScreen();
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void printText(int x, int y, const String& text, uint16_t color);
    void showMenu(int x, int y);
    void hideMenu(int x, int y);

    // Add these functions
    int getWidth(); 
    int getHeight();

    uint16_t backgroundColor;
    uint16_t effectColor;
    uint16_t menuColor;

private:
    Arduino_RGB_Display* gfx;
};

#endif

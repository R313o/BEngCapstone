#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino_GFX_Library.h>
//#include <U8g2lib.h>
#include "point.h"
//#include <string>
//#include "touch.h"

class display
{
public:

    display();
    void start();
    void fill(uint16_t colour);
    void setBrightness(int brightness);
    void drawTriangle(point point0, point point1, point point2, uint16_t colour);
    void drawRectangle(point center, int width, int height, uint16_t colour);
    void drawRoundedRectangle(point center, int width, int height, int roundRadius, uint16_t colour);
    void drawBorderedRoundedRectangle(point center, int width, int height, int roundRadius, int borderWidth, uint16_t mainColour, uint16_t borderColour);
    void drawCircle(point center, int radius, uint16_t colour);
    void drawLine(point start, point end, uint16_t colour);
    void drawThickLine(point start, point end, int lineThickness, uint16_t colour);
    void drawArc(point center, int startAngle, int endAngle, int radiusX, int radiusY, int width, uint16_t colour);
    void printText(point center, int fontSize, const String &buttonText, uint16_t colour);
    void update();

    point screenSize;
private:

    Arduino_ESP32RGBPanel *screen;
    Arduino_G *buffer;
    Arduino_GFX *canvas;
    int backlightPin;

};

#endif
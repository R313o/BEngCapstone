#include "display.h"



display::display()
{
    screenSize = {800,480};

    screen = new Arduino_ESP32RGBPanel(
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
    9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
    15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */,
    //0 /* hsync_polarity */, 8 /* hsyncFrontPorch */, 2 /* hsyncPulseWidth */, 43 /* hsyncBackPorch */,
    //0 /* vsyncPolarity */, 8 /* vsyncFrontPorch */, 2 /* vsyncPulseWidth */, 12 /* vsyncBackPorch */,
    0 /* hsync_polarity */, 210 /* hsyncFrontPorch */, 30 /* hsyncPulseWidth */, 16 /* hsyncBackPorch */,
    0 /* vsyncPolarity */, 22 /* vsyncFrontPorch */, 13 /* vsyncPulseWidth */, 10 /* vsyncBackPorch */,
    1 /* pclkActiveNeg */, 16000000 /* preferSpeed */
    );

    buffer = new Arduino_RGB_Display(
    screenSize.x /* width */, screenSize.y /* height */, screen, 0 /* rotation */, false /* auto_flush */
    );

    canvas = new Arduino_Canvas(screenSize.x  /* width */, screenSize.y  /* height */, buffer);

    backlightPin = 2;
}

void display::start()
{
    canvas->begin();

    //touch_init();

    pinMode(backlightPin, OUTPUT);

    setBrightness(200);
}

void display::fill(uint16_t colour)
{
    canvas->fillScreen(colour);
}

void display::setBrightness(int brightness)
{
    analogWrite(backlightPin, brightness);
}

void display::drawTriangle(point point0, point point1, point point2, uint16_t colour)
{
    canvas->fillTriangle(point0.x,point0.y,point1.x,point1.y,point2.x,point2.y,colour);
}

void display::drawRectangle(point center, int width, int height, uint16_t colour)
{
    canvas->fillRect(center.x-width/2, center.y-height/2, width, height, colour);
}

void display::drawRoundedRectangle(point center, int width, int height, int roundRadius, uint16_t colour)
{
    canvas->fillRoundRect(center.x-width/2, center.y-height/2, width, height, roundRadius, colour);
}

void display::drawBorderedRoundedRectangle(point center, int width, int height, int roundRadius, int borderWidth, uint16_t mainColour, uint16_t borderColour)
{
    drawRoundedRectangle( center, width, height, roundRadius, borderColour);
    drawRoundedRectangle( center, width-2*borderWidth, height-2*borderWidth, roundRadius-borderWidth, mainColour);
}

void display::drawCircle(point center, int radius, uint16_t colour)
{
    canvas->fillCircle(center.x, center.y, radius, colour); 
}

void display::drawLine(point start, point end, uint16_t colour)
{
    canvas->drawLine(start.x, start.y, end.x, end.y, colour);
}

void display::drawThickLine(point start, point end, int lineThickness, uint16_t colour)
{
    point len = {end.x-start.x,end.y-start.y};
    float mag = sqrt(pow(len.x,2) + pow(len.y,2));
    point offset = {(len.y/mag)*(float)0.5*lineThickness,(len.x/mag)*(float)0.5*lineThickness };
    point point1 = {start.x - offset.x , start.y + offset.y};
    point point2 = {start.x + offset.x , start.y - offset.y};
    point point3 = {end.x + offset.x , end.y - offset.y};
    point point4 = {end.x - offset.x , end.y + offset.y};
    //canvas->fillTriangle(point1.x,point1.y,point2.x,point2.y,point3.x,point3.y,colour);
    //canvas->fillTriangle(point1.x,point1.y,point4.x,point4.y,point3.x,point3.y,colour);
    drawTriangle(point1, point2, point3, colour);
    drawTriangle(point1, point4, point3, colour);
}

void display::drawArc(point center, int startAngle, int endAngle, int radiusX, int radiusY, int width, uint16_t colour)
{
    byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Draw colour blocks every inc degrees
     for (int i = startAngle; i < endAngle; i += inc) 
     {
        // Calculate pair of coordinates for segment start
        point p0,p1,p2,p3;
        float sx = cos((i - 90) * 0.0174533);
        float sy = sin((i - 90) * 0.0174533);
        p0.x = sx * (radiusX - width) + center.x;
        p0.y = sy * (radiusY - width) + center.y;
        p1.x = sx * radiusX + center.x;
        p1.y = sy * radiusY + center.y;

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * 0.0174533);
        float sy2 = sin((i + seg - 90) * 0.0174533);
        p2.x = sx2 * (radiusX - width) + center.x;
        p2.y = sy2 * (radiusY - width) + center.y;
        p3.x = sx2 * radiusX + center.x;
        p3.y = sy2 * radiusY + center.y;

        drawTriangle(p0, p1, p2, colour);
        drawTriangle(p1, p2, p3, colour);
    }
}

void display::printText(point center, int fontSize, const String &buttonText, uint16_t colour)
{
    canvas->setTextColor(colour);
    canvas->setTextSize(fontSize);
    //canvas->setFont(u8g2_font_unifont_t_cjk);
    canvas->setCursor(center.x-buttonText.length()*fontSize*6/2 ,center.y-fontSize*7/2);
    canvas->print(buttonText);
}

void display::update()
{
    canvas->flush();
}
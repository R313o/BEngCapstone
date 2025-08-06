#include "ourui.h"
#include "display.h"

ourui::ourui(display* disp)
{
    _disp = disp; // store the pointer
    //_disp->drawRectangle({40, 20}, 80, 40, 0x03fc41);
}

void ourui::draw() {
    uint16_t colour = hexToRGB565(0xCA0EEB);
    uint16_t black = hexToRGB565(0x000000);
    
    _disp->drawBorderedRoundedRectangle({40,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+80,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+2*80,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+3*80,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+4*80,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+5*80,20}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({40+6*80,20}, 80, 40, 5, 2, colour, black);

    _disp->drawBorderedRoundedRectangle({40,480-20}, 80, 40, 5, 2, colour, black);

    _disp->drawBorderedRoundedRectangle({40+80,240}, 80, 40, 5, 2, colour, black);
    _disp->drawBorderedRoundedRectangle({800-80,240}, 80, 40, 5, 2, colour, black);


    _disp->printText({40,20}, 2, amp, black);
    _disp->printText({40+80,20}, 2, chorus, black);
    _disp->printText({40+2*80,20}, 2, phasor, black);
    _disp->printText({40+3*80,20}, 2, delay, black);
    _disp->printText({40+4*80,20}, 1, overdrive, black);
    _disp->printText({40+5*80,20}, 2, reverb, black);
    _disp->printText({40+6*80,20}, 2, mixer, black);

    _disp->printText({40,480-20}, 2, update, black);

    _disp->printText({40+80,240}, 2, input, black);
    _disp->printText({800-80,240}, 2, output, black);






} 

uint16_t ourui::hexToRGB565(uint32_t hexColor) {
    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8) & 0xFF;
    uint8_t b = hexColor & 0xFF;

    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

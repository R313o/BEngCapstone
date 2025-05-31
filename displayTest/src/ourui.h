#include <Arduino_GFX_Library.h>
#include "point.h"
#include "display.h"

class ourui
{
    public:
        ourui(display* disp); // fix: specify the parameter type
        void draw();
        uint16_t hexToRGB565(uint32_t hexColor);
    private:
        String amp = "Amp";
        String chorus = "Chorus";
        String phasor = "Phasor";
        String delay = "Delay";
        String overdrive = "Overdrive";
        String reverb = "Reverb";
        String mixer = "Mixer";
        String input = "Input";
        String output = "Output";
        String update = "Update";
        display* _disp; // fix: store the pointer to use it later
};
#ifndef COLOURS_H
#define COLOURS_H

//#include <Arduino.h>

#define MEDIUM_ORANGE RGB565(252, 163, 17)
#define DARK_BLUE RGB565(20, 33, 61)
#define LIGHT_GREY RGB565(229, 229, 229)
#define OFF_BLACK RGB565(10, 14, 10)
#define MEDIUM_OLIVE RGB565(105, 117, 101)
#define LIGHT_CREAM RGB565(236, 223, 204)
#define DARK_BROWN RGB565(60, 61, 55)
#define LIGHT_TEAL RGB565(0x00, 0xAB, 0xBD)
#define MEDIUM_TEAL RGB565(0x00, 0x99, 0xDD)
#define DARK_TEAL RGB565(0x02, 0x6E, 0x81)
#define MEDIUM_YELLOW RGB565(0xFF, 0x99, 0x33)
#define MEDIUM_GREY RGB565(0xA1, 0xC7, 0xE0)

#define MEDIUM_PINK RGB565(0xF2, 0x66, 0x8B)

#define DARK_NAVY RGB565(0x02, 0x14, 0x20)
#define FOREST_GREEN RGB565(0x00, 0x5C, 0x53)
#define LIME_GREEN RGB565(0x9F, 0xC1, 0x31)
#define LEMON_YELLOW RGB565(0xDB, 0xF2, 0x27)
#define LIGHT_BEIGE RGB565(0xD6, 0xD5, 0x8E)


struct palette
{
    uint16_t main;
    uint16_t background;
    uint16_t accent1;
    uint16_t accent2;
    uint16_t accent3;
};


#endif

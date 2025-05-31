#ifndef EFFECT_H
#define EFFECT_H

#include <Arduino.h>
#include "display.h"

class effect {
public:
    effect(int startX, int startY, String text, display& disp);
    void draw();
    bool checkTouch(int touchX, int touchY);
    void startDrag(int touchX, int touchY);
    void stopDrag();
    void moveTo(int touchX, int touchY);
    void showMenu();
    void hideMenu();

private:
    int x, y, width, height;
    String label;
    bool isDragging;
    bool wasDragged;
    bool isPressed;
    int dragOffsetX, dragOffsetY;
    int startX, startY;
    display& disp;
    bool menuActive;
};

#endif

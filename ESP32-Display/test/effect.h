#ifndef EFFECT_H
#define EFFECT_H

#include <Arduino_GFX_Library.h>

extern Arduino_RGB_Display *gfx; // This will reference the gfx object from the main file

class Effect {
public:
    int x, y, width, height;
    String label;
    bool isDragging;
    bool wasDragged;
    int dragOffsetX, dragOffsetY;
    Effect* inputs[5];  // Array for input effect pointers
    Effect* outputs[5]; // Array for output effect pointers
    int inputCount, outputCount;
    bool menuActive;

    Effect(int startX, int startY, String text);
    
    void draw();
    bool isTouched(int touchX, int touchY);
    void startDrag(int touchX, int touchY);
    void stopDrag();
    void moveTo(int touchX, int touchY);
    void showMenu();
    void hideMenu();
    bool isDeleteTouched(int touchX, int touchY);
    bool isAddInputTouched(int touchX, int touchY);
    bool isAddOutputTouched(int touchX, int touchY);
    void addInput(Effect* input);
    void addOutput(Effect* output);
    void drawConnections();
    void drawConnection(Effect* otherEffect);
};

#endif

#include "effect.h"

effect::effect(int startX, int startY, String text, display& disp)
    : x(startX), y(startY), label(text), disp(disp) {
    width = 100;
    height = 50;
    isDragging = false;
    wasDragged = false;
    isPressed = false;
    dragOffsetX = 0;
    dragOffsetY = 0;
    menuActive = false;
}

void effect::draw() {
    disp.fillRect(x, y, width, height, disp.effectColor);
    disp.printText(x + 15, y + 15, label, 0xFFFF);
}

bool effect::checkTouch(int touchX, int touchY) {
    if (touchX >= x && touchX <= x + width && touchY >= y && touchY <= y + height) {
        startDrag(touchX, touchY);
        return true;
    }
    return false;
}

void effect::startDrag(int touchX, int touchY) {
    isDragging = true;
    dragOffsetX = touchX - x;
    dragOffsetY = touchY - y;
    hideMenu();
}

void effect::stopDrag() {
    isDragging = false;
}

void effect::moveTo(int touchX, int touchY) {
    disp.fillRect(x, y, width, height, disp.backgroundColor);
    x = touchX - dragOffsetX;
    y = touchY - dragOffsetY;
    draw();
}

void effect::showMenu() {
    menuActive = true;
    disp.showMenu(x, y);
}

void effect::hideMenu() {
    if (!menuActive) return;
    menuActive = false;
    disp.hideMenu(x, y);
}

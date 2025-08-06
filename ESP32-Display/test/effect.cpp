#include "effect.h"

extern uint16_t backgroundColor;  // Reference from the main file
extern uint16_t effectColor;      // Reference from the main file
extern uint16_t menuColor;        // Reference from the main file
extern uint16_t lineColor;        // Reference from the main file

int menuOffsetX = 120;
int menuOffsetY = 0;
int menuButtonWidth = 100;
int menuButtonHeight = 50;
int menuSpacing = 10;

Effect::Effect(int startX, int startY, String text) {
    x = startX;
    y = startY;
    width = 100;
    height = 50;
    label = text;
    isDragging = false;
    wasDragged = false;
    dragOffsetX = 0;
    dragOffsetY = 0;
    inputCount = 0;
    outputCount = 0;
    menuActive = false;
}

void Effect::draw() {
    gfx->fillRect(x, y, width, height, effectColor); // Dark grey effect button
    gfx->setCursor(x + 15, y + 15);
    gfx->setTextColor(0xFFFF); // White text
    gfx->setTextSize(2);
    gfx->print(label);
}

bool Effect::isTouched(int touchX, int touchY) {
    return (touchX >= x && touchX <= x + width && 
            touchY >= y && touchY <= y + height);
}

void Effect::startDrag(int touchX, int touchY) {
    isDragging = true;
    wasDragged = false;
    dragOffsetX = touchX - x;
    dragOffsetY = touchY - y;
    hideMenu();  // Hide the menu when dragging starts
}

void Effect::stopDrag() {
    isDragging = false;
}

void Effect::moveTo(int touchX, int touchY) {
    if (!isDragging) return;

    // Erase the previous position
    gfx->fillRect(x, y, width, height, backgroundColor); // Erase with background color

    // Update the position
    x = touchX - dragOffsetX;
    y = touchY - dragOffsetY;

    // Mark that it was dragged
    wasDragged = true;

    // Draw the button in the new position
    draw();

    // Redraw any connected input/output lines
    drawConnections();
}

void Effect::showMenu() {
    gfx->fillRect(x + menuOffsetX, y + menuOffsetY, menuButtonWidth, menuButtonHeight, menuColor); // Pale orange Delete button
    gfx->setCursor(x + menuOffsetX + 15, y + menuOffsetY + 15);
    gfx->setTextColor(0x0000);
    gfx->setTextSize(2);
    gfx->print("Delete");

    gfx->fillRect(x + menuOffsetX, y + menuOffsetY + menuButtonHeight + menuSpacing, menuButtonWidth, menuButtonHeight, menuColor); // Pale orange Add Input button
    gfx->setCursor(x + menuOffsetX + 15, y + menuOffsetY + menuButtonHeight + menuSpacing + 15);
    gfx->setTextColor(0x0000);
    gfx->print("Add In");

    gfx->fillRect(x + menuOffsetX, y + menuOffsetY + 2 * (menuButtonHeight + menuSpacing), menuButtonWidth, menuButtonHeight, menuColor); // Pale orange Add Output button
    gfx->setCursor(x + menuOffsetX + 15, y + menuOffsetY + 2 * (menuButtonHeight + menuSpacing) + 15);
    gfx->setTextColor(0x0000);
    gfx->print("Add Out");

    menuActive = true;
}

void Effect::hideMenu() {
    gfx->fillRect(x + menuOffsetX, y + menuOffsetY, menuButtonWidth, 3 * (menuButtonHeight + menuSpacing), backgroundColor); 
    menuActive = false;
}

bool Effect::isDeleteTouched(int touchX, int touchY) {
    return (menuActive && 
            touchX >= x + menuOffsetX && touchX <= x + menuOffsetX + menuButtonWidth &&
            touchY >= y + menuOffsetY && touchY <= y + menuOffsetY + menuButtonHeight);
}

bool Effect::isAddInputTouched(int touchX, int touchY) {
    return (menuActive &&
            touchX >= x + menuOffsetX && touchX <= x + menuOffsetX + menuButtonWidth &&
            touchY >= y + menuOffsetY + menuButtonHeight + menuSpacing && touchY <= y + menuOffsetY + 2 * (menuButtonHeight + menuSpacing));
}

bool Effect::isAddOutputTouched(int touchX, int touchY) {
    return (menuActive &&
            touchX >= x + menuOffsetX && touchX <= x + menuOffsetX + menuButtonWidth &&
            touchY >= y + menuOffsetY + 2 * (menuButtonHeight + menuSpacing) && touchY <= y + menuOffsetY + 3 * (menuButtonHeight + menuSpacing));
}

void Effect::addInput(Effect* input) {
    if (inputCount < 5) {
        inputs[inputCount++] = input;
        drawConnection(input);  // Draw the connection line
    }
}

void Effect::addOutput(Effect* output) {
    if (outputCount < 5) {
        outputs[outputCount++] = output;
        drawConnection(output);  // Draw the connection line
    }
}

void Effect::drawConnections() {
    for (int i = 0; i < inputCount; i++) {
        drawConnection(inputs[i]);
    }
    for (int i = 0; i < outputCount; i++) {
        drawConnection(outputs[i]);
    }
}

void Effect::drawConnection(Effect* otherEffect) {
    gfx->drawLine(x + width / 2, y + height / 2, otherEffect->x + otherEffect->width / 2, otherEffect->y + otherEffect->height / 2, lineColor);
}

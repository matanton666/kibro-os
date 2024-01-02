#include "Keyboard.h"

Keyboard keyboard;

void Keyboard::handleKeyboard(uint8_t scancode) {

    switch (scancode) {
    case L_SHIFT:
        leftShift = true;
        return;
    case L_SHIFT + 0x80:
        leftShift = false;
        return;
    case R_SHIFT:
        rightShift = true;
        return;
    case R_SHIFT + 0x80:
        rightShift = false;
        return;
    case CTRL:
        ctrl = true;
        return;
    case CTRL + 0x80:
        ctrl = false;
        return;
    case ALT:
        alt = true;
        return;
    case ALT + 0x80:
        alt = false;
        return;
    case CAPS_LOCK:
        capsLock = !capsLock;
        return;
    case BACKSPACE:
        clearLastChar();
        return;
    case ENTER:
        print("\n");
        return;
    case SPACE_BAR:
        print(' ');
        return;
    case TAB:
        print("    ");
        return;
    }

    char ch = translateScanCode(scancode, rightShift || leftShift, capsLock);
    if (ch != 0) {
        print(ch);
    }

}

char Keyboard::translateScanCode(uint8_t scancode, bool shift, bool capsLock)
{
    if (scancode > 58) 
        return 0;
  
    int ch = (int)ASCIITable[scancode];

    if (shift)
        return isAlpha(ch) ? toUpper(ch) : shiftDigit(ch);
    else if (capsLock)
        return toUpper(ch);

    return ch;
}

void keyboardHandler(uint8_t scancode)
{
    keyboard.handleKeyboard(scancode);
}

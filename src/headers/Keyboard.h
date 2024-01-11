#pragma once
#include <stdint.h>
#include "serial.h"
#include "std.h"
#include "screen.h"
#include "ctype.h"

// special keys scancodes
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define CAPS_LOCK 0x3A
#define CTRL 0x1D
#define ALT 0x38
#define ENTER 0x1C
#define SPACE_BAR 0x39
#define TAB 0xF
#define BACKSPACE 0xE

const char ASCIITable[] = { 0 ,  0 , '1', '2','3', '4', '5', '6','7', '8', '9', '0','-', '=',
							0 ,  0 ,'q', 'w', 'e', 'r','t', 'y', 'u', 'i','o','p', '[', ']', 0 ,
							0 , 'a', 's','d', 'f', 'g', 'h','j', 'k', 'l', ';','\'','`',  0 ,
							'\\','z', 'x', 'c', 'v','b', 'n', 'm', ',','.', '/',  0 , '*', 0 , ' ' };

class Keyboard
{
private:
	bool leftShift, rightShift, capsLock = false, alt, ctrl;
public:
	// translate scancode and print it to the screen with charicter manipulation (shift, capslock, ect.)
	void handleKeyboard(uint8_t scancode);

	// translate a scancode to the coresponding charicter
	char translateScanCode(uint8_t scanCode, bool shift, bool capsLock);

};

extern Keyboard keyboard;

// wrapper function for keyboard.handleKeyboard
void keyboardHandler(uint8_t scancode);




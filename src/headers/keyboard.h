/*
handel keyboard input, translate scancodes to charicters
*/

#pragma once
#include <stdint.h>
#include "serial.h"
#include "std.h"
#include "screen.h"
#include "ctype.h"
#include "queue.h"

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

#define KEYBOARD_BUFFER_SIZE 256

const char ASCIITable[] = { 0 ,  0 , '1', '2','3', '4', '5', '6','7', '8', '9', '0','-', '=',
							// 8 is backspace
							'\b',  '\t' ,'q', 'w', 'e', 'r','t', 'y', 'u', 'i','o','p', '[', ']', '\n' ,
							0 , 'a', 's','d', 'f', 'g', 'h','j', 'k', 'l', ';','\'','`',  0 ,
							'\\','z', 'x', 'c', 'v','b', 'n', 'm', ',','.', '/',  0 , '*', 0 , ' ', 0 };


class Keyboard
{
private:
	bool _leftShift, _rightShift, _capsLock = false, alt, ctrl;

	char _buffer[KEYBOARD_BUFFER_SIZE] = { 0 }; // buffer to hold the input for the queue
	int _writeIndex = 0;
	int _readIndex = 0;

	// translate scancode and print it to the screen with charicter manipulation (shift, capslock, ect.)
	void checkSpecialChar(uint8_t scancode);

	// translate a scancode to the coresponding charicter
	char translateScanCode(uint8_t scanCode, bool shift, bool capsLock);

public:

	void reset();

	// translate the scancode and insert to the keyboard queue
	void sendToQueue(uint8_t scancode);

	// get the first charicter from the keyboard queue (will return 0 if queue empty)
	char getFromQueue();

	// check if the keyboard queue is empty
	bool hasBeenPressed();


	void printScancode(uint8_t scancode);

	char getChar(uint8_t scancode);



	// get the state of the left shift key
	bool leftShift() { return _leftShift; }

	// get the state of the right shift key
	bool rightShift() { return _rightShift; }

	// get the state of the caps lock key
	bool capsLock() { return _capsLock; }

	// get the state of the alt key
	bool altKey() { return alt; }

	// get the state of the ctrl key
	bool ctrlKey() { return ctrl; }


};

extern Keyboard keyboard;



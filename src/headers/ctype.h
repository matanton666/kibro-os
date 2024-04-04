/*
charicter manipulation and checking functions
*/
#pragma once

// number or letter
bool isAlNum(char ch);
// letter
bool isAlpha(char ch);
// space or tab
bool isBlank(char ch);
// control charicter (ex. backspace, enter, ect.)
bool isCntrl(char ch);
// number
bool isDigit(char ch);
// charicter that can be seen (excluding space and enter)
bool isGraph(char ch);
// lower case letter
bool isLower(char ch);
// charicter that can be seen (excluding enter)
bool isPrint(char ch);
// space
bool isSpace(char ch);
// upper case letter
bool isUpper(char ch);
// hex digit
bool isXDigit(char ch);

// convert to lower case
char toLower(char ch);
// convert to upper case
char toUpper(char ch);
// convert digit to shifted digit (ex. '1' -> '!') 
char shiftDigit(char ch);
/*
our implementation of a C library
*/
#pragma once

bool isAlNum(char ch);
bool isAlpha(char ch);
bool isBlank(char ch);
bool isCntrl(char ch);
bool isDigit(char ch);
bool isGraph(char ch);
bool isLower(char ch);
bool isPrint(char ch);
bool isSpace(char ch);
bool isUpper(char ch);
bool isXDigit(char ch);

char toLower(char ch);
char toUpper(char ch);

//shift + digits 
char shiftDigit(char ch);
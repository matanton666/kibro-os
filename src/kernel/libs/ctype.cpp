#include "../../headers/ctype.h"

bool isAlNum(char ch)
{
	return (isAlpha(ch) || isDigit(ch));
}
bool isAlpha(char ch)
{
	return (isUpper(ch) || isLower(ch));
}
bool isBlank(char ch)
{
	return (ch == ' ' || ch == '\t');
}
bool isCntrl(char ch)
{
	return ((ch >= 0 && ch <= 31) || ch == 127);
}
bool isDigit(char ch)
{
	return (ch >= '0' && ch <= '9');
}
bool isGraph(char ch)
{
	return isPrintable(ch) && !isSpace(ch);
}
bool isLower(char ch)
{
	return (ch >= 'a' && ch <= 'z');
}
bool isPrintable(char ch)
{
	return (ch >= ' ' && ch <= '~');
}
bool isSpace(char ch)
{
	return ch == ' ';
}
bool isUpper(char ch)
{
	return (ch >= 'A' && ch <= 'Z');
}
bool isXDigit(char ch)
{
	return (isDigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'));
}

char toLower(char ch)
{
	if (isLower(ch))
		return (char)(ch + 32);
	return ch;
}
char toUpper(char ch)
{
	if (isLower(ch))
		return (char)(ch - 32);
	return ch;
}

char shiftNonAlpha(char ch)
{
    if (isPrintable(ch)) {
        switch (ch) {
        case '1':
            return '!';
        case '2':
            return '@';
        case '3':
            return '#';
        case '4':
            return '$';
        case '5':
            return '%';
        case '6':
            return '^';
        case '7':
            return '&';
        case '8':
            return '*';
        case '9':
            return '(';
        case '0':
            return ')';
        case '[':
            return '{';
        case ']':
            return '}';
        case '/':
            return '?';
        case ',':
            return '<';
        case '.':
            return '>';
        case ';':
            return ':';
        case '\'':
            return '"';
        case '`':
            return '~';
        case '\\':
            return '|';
        case '-':
            return '_';
        case '=':
            return '+';
        }

    }
    return ch;
}	
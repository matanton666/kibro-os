#include "std.h"

void* memcpy(void* dest, const void* src, size_t n) {
	if (dest == src) {
	// copying to the same location
	return dest;
	}

	char* d = (char*)dest;
	const char* s = (const char*)src;

	// copy n bytes from src to dest
	for (size_t i = 0; i < n; ++i) {
		d[i] = s[i];
	}

	return dest;
}

int memcmp(const void* lhs, const void* rhs, size_t count)
{
	unsigned char* x = (unsigned char*)lhs;
	unsigned char* y = (unsigned char*)rhs;

	if (lhs == rhs)
	{
		return 0;
	}

	int compare = 0;
	while (count > 0)
	{
		if (*x != *y)
		{
			compare = (*x > *y) ? 1 : -1;
			break;
		}
		count--;
		x++;
		y++;
	}
	return compare;
}

// convert from int to char* (ascii)
char* itoa(int num, char* str, int base) {
	int i = 0;
	bool isNegative = false;

	// negative number
	if (num < 0) {
		isNegative = true;
		num *= -1;
	}

	// convert int to string in wanted base in reverse
	do {
		int digit = num % base;
		str[i++] = (digit <= 9) ? digit + '0' : digit + 'A' - 10; // convert to hex if need
	} while ((num /= base) != 0);

	if (isNegative) {
		str[i++] = '-';
	}

	// reverse the string back
	int k = i - 1;
	for (int j = 0; j < k; ++j, --k) {
		char temp = str[j]; // swap first and last char
		str[j] = str[k];
		str[k] = temp;
	}

	str[i] = '\0';
	return str;
}



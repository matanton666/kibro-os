#include "../../headers/std.h"

uint32_t* bootInfoPtr = nullptr;

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

	if (lhs == rhs) // check if same location
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

void memset(void* dest, int ch, uint32_t count)
{
	if (ch == 0 && count > 16) // faster to set memory to zero with zeroMemory function
	{
		zeroMemory(dest, count);
		return;
	}

	char* addr = (char*)dest;
	for (int i = 0; i < count; i++)
	{
		addr[i] = ch;
	}
}

void zeroMemory(void* dest, uint32_t count)
{
	unsigned char* addr = (unsigned char*)dest;
	for (uint32_t i = 0; i < count; i++)
	{
		addr[i] ^= addr[i];
	}
}

uintptr_t align_up(uintptr_t address, uintptr_t alignment) {
    return (address + alignment - 1) & ~(alignment - 1);
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

char* uitoa(uint64_t num, char *str, int base)
{
	int i = 0;

	// convert int to string in wanted base in reverse
	do {
		int digit = num % base;
		str[i++] = (digit <= 9) ? digit + '0' : digit + 'A' - 10; // convert to hex if need
	} while ((num /= base) != 0);


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

char* ftoa(float num, char* str, int base) 
{
    // Extract integer and fractional parts
    int ipart = (int)num;
    float fpart = num - (float)ipart;

    // Convert integer part to string
    itoa(ipart, str, base);

    // If decimal places are required
	str[strlen(str)] = '.';  // Add decimal point

	for (int i = 0; i < 8; i++)
	{
		fpart *= base;
	}

	itoa((int)fpart, str + strlen(str), base);

	return str;
}

unsigned int atoi(const char* str)
{
	unsigned int result = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
			result = result * 10 + str[i] - '0';
	}
	return result;

}

uint32_t atoh(const char* str)
{
	uint32_t result = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
			result = result * 16 + str[i] - '0';
		else if (str[i] >= 'A' && str[i] <= 'F')
			result = result * 16 + str[i] - 'A' + 10;
		else if (str[i] >= 'a' && str[i] <= 'f')
			result = result * 16 + str[i] - 'a' + 10;
	}
	return result;
}

unsigned int strlen(const char* str)
{
	int i = 0;
	for(i = 0; str[i] != '\0'; i++) {
	}
	return i;
}

char* strcat(char* dest, const char* src) 
{
	if (dest == nullptr || src == nullptr) {
		return nullptr;
	}

	char* ptr = dest + strlen(dest);
	while (*src != '\0') {
		*ptr++ = *src++;
	}
	*ptr = '\0';

	return dest;
}

char* strcpy(char* dest, const char* src) 
{
	if (dest == nullptr || src == nullptr) {
		return nullptr;
	}

	char* ptr = dest;
	while (*src != '\0') {
		*ptr++ = *src++;
	}
	*ptr = '\0';

	return dest;
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 == *s2) {
		if (*s1 == '\0') {
			return 0;
		}
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %b0, %w1"
        :: "a"(value), "d"(port));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %w1, %b0"
		: "=a"(ret)
		: "d"(port));
	return ret;
}

void outl(uint16_t port, uint32_t value) 
{
	asm volatile("outl %0, %1"
		:
		: "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port) 
{
	uint32_t ret;
	asm volatile("inl %1, %0"
		: "=a"(ret)
		: "Nd"(port));
	return ret;
}


void ioWait()
{
	asm volatile("outb %%al, $0x80"
		:
		: "a"(0));
}

void cli()
{
	asm volatile("cli");
}

void sti()
{
	asm volatile("sti");
}

void getBootInfoAddressFromGrub() {
	if (bootInfoPtr == nullptr) {
		asm("movl %%ebx, %0;" : "=r"(bootInfoPtr));
	}
}
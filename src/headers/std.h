#pragma once
#include <stdint.h>
#include <stddef.h>


char* itoa(int num, char *str, int base);
void* memcpy(void *dest, const void *src, size_t n);
int memcmp(const void* lhs, const void* rhs, size_t count);
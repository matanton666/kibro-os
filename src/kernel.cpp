#include <stdint.h>

#if defined(__linux__)
#error "This is not a cross-compiler"
#endif

extern "C" void kernel_main(void) {
	while (true)
	{

	}
}

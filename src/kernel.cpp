#include <stdint.h>
#include "headers/serial.h"
#include "headers/screen.h"
#include "headers/std.h"


extern "C" void kernel_main(void) {
	init_serial();
	write_serial((char*)"kernel booted");

	struct FramebufferInfo* fb = initFramebuffer();
	write_serial((char*)"Framebuffer initialized");

	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			drawPixel(fb, i, j, 0x00ff0000);
		}
	}
	
	
	while (true)
	{
		 
	}
}


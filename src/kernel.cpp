#include <stdint.h>
#include "headers/serial.h"
#include "headers/screen.h"
#include "headers/std.h"


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;


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


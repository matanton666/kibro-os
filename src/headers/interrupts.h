#pragma once

#include "screen.h"


struct interrupt_frame;
__attribute__((interrupt)) void pagefaultHandler(struct interrupt_frame* frame);

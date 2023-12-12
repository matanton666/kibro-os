#pragma once
/// these are the ISRs of the IDT
#include "screen.h"


struct InterruptFrame {
    unsigned int ip;
    unsigned int cs;
    unsigned int flags;
    unsigned int sp;
    unsigned int ss;
};

// error code needs to be unsigned int because this is 32-bit os

__attribute__((interrupt)) void generalFault(struct InterruptFrame* frame);
__attribute__((interrupt)) void generalFaultWithErrCode(struct InterruptFrame *frame, unsigned int errorCode);

__attribute__((interrupt)) void pagefaultHandler(struct InterruptFrame *frame, unsigned int errorCode);
__attribute__((interrupt)) void doubleFault(struct InterruptFrame *frame, unsigned int errorCode);

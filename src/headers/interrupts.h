#pragma once
/// these are the ISRs of the IDT
#include "screen.h"

typedef bool bit_t;

struct InterruptFrame {
    unsigned int ip;
    unsigned int cs;
    unsigned int flags;
    unsigned int sp;
    unsigned int ss;
} __attribute__((packed));

struct PageFalutError { // make sure its alligned correctly and has right size
    bit_t present : 1;
    bit_t write : 1;
    bit_t user : 1;
    bit_t reserved_write : 1;
    bit_t instruction_fetch : 1;
    bit_t protection_key : 1;
    bit_t shadow_stack : 1;
    unsigned char reserved : 8;
    bit_t sgx : 1;
    uint16_t reserved2 : 15;
} __attribute__((packed));


struct SelectorError {
    bit_t external : 1;
    unsigned char table : 2;
    uint16_t index : 13;
    uint16_t reserved : 15;
} __attribute__((packed));

// error code needs to be unsigned int because this is 32-bit os

__attribute__((no_caller_saved_registers)) void printException(unsigned int exceptionNumber, unsigned int errorCode);
__attribute__((no_caller_saved_registers)) void printSelectorError(SelectorError* err);


__attribute__((interrupt)) void generalFault(struct InterruptFrame* frame);
__attribute__((interrupt)) void generalFaultWithErrCode(struct InterruptFrame *frame, unsigned int errorCode);

__attribute__((interrupt)) void devideByZeroHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void overflowHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void boundRangeExceededHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void invalidOpcodeHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void deviceNotAvailableHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void doubleFaultHandler(struct InterruptFrame *frame);
__attribute__((interrupt)) void invalidTSS_Handler(struct InterruptFrame *frame, unsigned int errorCode);
__attribute__((interrupt)) void segmentNotPresentHandler(struct InterruptFrame *frame, unsigned int errorCode);
__attribute__((interrupt)) void stackSegmentFaultHandler(struct InterruptFrame *frame, unsigned int errorCode);
__attribute__((interrupt)) void generalProtectionFaultHandler(struct InterruptFrame *frame, unsigned int errorCode);
__attribute__((interrupt)) void pagefaultHandler(struct InterruptFrame *frame, unsigned int errorCode);


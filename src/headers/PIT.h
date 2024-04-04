#pragma once

#include "std.h"
#include "serial.h"

#define PIT_PORT 0x40


class PIT {

private:
    const uint32_t _BASE_FEQUENCY = 1193182; // frequency of the pit chip crystal in herz
    const uint32_t _MS_IN_SEC = 1000; // number of miliseconds in a second

    //! NOTE: because use of 32 bit time in ms, max time online is 49.7 days / ~7 weeks
    uint32_t _time_since_boot = 0; // number of ms passed since boot
    uint32_t _running_frequency = 0;
    uint16_t _divisor = 0; // divides the frequency on the chip for programable frequencys

    // set the divisor on the pit chip 
    void setDivisor(uint16_t divisor);


public:

    // init the PIC with a frequency of 1000 hz (1000 ticks per second / one tick per milisecond)
    void init();

    // wait for x seconds (accureate to +- 10 miliseconds)
    void sleepS(float seconds);

    // wait for x miliseconds (accureate to +- 10 miliseconds)
    void sleepMS(uint32_t miliseconds);

    //!!!DONT USE FUNCTION!!! (ONLY IN INTERRUPT!) using elsewhere can cause unknown behavior to timer and system
    void tick();

    uint32_t getTimeSinceBoot();

    // get the frequency in hz
    uint32_t getFrequency();

    // set the frequency in hz
    void setFrequency(uint32_t frequency);
};


extern PIT pit;



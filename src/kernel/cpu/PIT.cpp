#include "../../headers/PIT.h"


PIT pit;


void PIT::setDivisor(uint16_t divisor)
{
    asm volatile("cli");
    divisor = divisor < 100 ?  100 : divisor; // make sure divisor is above 100 so interrupt wont be to fast
    _divisor = divisor;
    _running_frequency = _BASE_FEQUENCY / _divisor;

    // set divisor on PIT chip
    outb(PIT_PORT, (uint8_t)(divisor & 0x00ff)); // pass the first 8 bits
    ioWait();
    outb(PIT_PORT, (uint8_t)((divisor & 0xff00) >> 8)); // pass the first 8 bits
}

void PIT::init()
{
    setFrequency(_MS_IN_SEC / 10); // tick once every 10 miliseconds
}


void PIT::sleepS(float seconds)
{
    sleepMS((float)seconds * _MS_IN_SEC);
}


void PIT::sleepMS(uint32_t miliseconds)
{
    uint32_t start_time = _time_since_boot;
    while (_time_since_boot < start_time + miliseconds)
    {
        asm volatile("hlt");
    }
}


uint32_t PIT::getFrequency()
{
    return _running_frequency;
}


void PIT::setFrequency(uint32_t frequency)
{
    frequency = frequency < 1 ? 1 : frequency; // minimum of 1 for frequency 
    frequency = frequency > _BASE_FEQUENCY ? _BASE_FEQUENCY : frequency; // maximum of _BASE_FEQUENCY for frequency
    setDivisor(_BASE_FEQUENCY / frequency);
}

void PIT::tick() 
{
    _time_since_boot += _MS_IN_SEC / _running_frequency; // add amount of ms passed
}

uint32_t PIT::getTimeSinceBoot()
{
    return _time_since_boot;
}

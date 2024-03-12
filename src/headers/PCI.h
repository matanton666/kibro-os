#pragma once

#include "std.h"
#include "serial.h"
#include "screen.h"
#include "pciDescriptors.h"
#include "ahci.h"

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC


class PCI
{
private:
    uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

    void checkDevice(uint8_t bus, uint8_t device);

    void checkFunction(uint8_t bus, uint8_t device, uint8_t function);
    
public:

    void checkAllBuses();

    static void printAvailableDevices();

    // template function to get header from bus
    template <typename T>
    T getPciHeader(uint8_t bus, uint8_t device, uint8_t function)
    {
        uint16_t dv_header[sizeof(T)] = {0};

        for (int i = 0; i < sizeof(T); i+=2)
        {
            dv_header[i/2] = pciConfigReadWord(bus, device, function, i);
        }

        return *(T*)dv_header;
    }
};


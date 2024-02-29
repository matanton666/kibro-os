#pragma once

#include "std.h"
#include "serial.h"

struct RSDP
{
    
};



struct ACPIV1
{
    uint32_t type; // 15
    uint32_t size; // size of the structure
    uint8_t rsdp[0]; // pointer to the RSDP
};

struct RSDPV1
{
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
}__attribute__((packed));


void init();
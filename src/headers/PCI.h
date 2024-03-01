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

struct RSDPV1 // root system descryptor ptr
{
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address; // root system descryptor table
}__attribute__((packed));

struct ACPI_STDHeader
{
    uint8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
}__attribute__((packed));

struct RSDT
{
    ACPI_STDHeader header;
    uintptr_t next_RSDT_ptr;
}__attribute__((packed));



// struct MCFG
// {
//     RSDTHeader header;
//     uint64_t reserved;
//     // uint64_t base_address;
//     // uint16_t segment_group;
//     // uint8_t start_bus_number;
//     // uint8_t end_bus_number;
//     // uint32_t reserved2;
// }__attribute__((packed));

void init();
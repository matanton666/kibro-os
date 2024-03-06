#pragma once

#include "std.h"
#include "serial.h"
#include "screen.h"
#include "pciDescriptors.h"

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


#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC


struct PciDeviceHeder
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t BIST;
}__attribute__((packed));





void init();

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void checkDevice(uint8_t bus, uint8_t device);


void checkFunction(uint8_t bus, uint8_t device, uint8_t function);

void checkAllBuses();

PciDeviceHeder getDeviceHeader(uint8_t bus, uint8_t device, uint8_t function);
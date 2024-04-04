#pragma once
#include "std.h"


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

struct PciHeader0
{
    PciDeviceHeder header;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_address;
    uint8_t capabilities_pointer;
    uint8_t reserved[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
}__attribute__((packed));



const char* GetVendorName(uint16_t vendorID);


const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);


const char* MassStorageControllerSubclassName(uint8_t subclassCode);

const char* SerialBusControllerSubclassName(uint8_t subclassCode);


const char* BridgeDeviceSubclassName(uint8_t subclassCode);


const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);


const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);


const char* getDeviceClassName(uint8_t classCode);
#include "../../headers/PCI.h"


uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) 
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

void checkDevice(uint8_t bus, uint8_t device) 
{
    uint8_t function = 0;
    uint16_t vendorID;
    uint8_t headerType;
 
    vendorID = pciConfigReadWord(bus, device, function, 0);
    if (vendorID != 0xFFFF) // Device exists
    {
        PciDeviceHeder dv_header = getPciHeader<PciDeviceHeder>(bus, device, function);
        checkFunction(bus, device, function);
        headerType = dv_header.header_type & 0x7F;
        if( (headerType & 0x80) != 0) {
            write_serial("It's a multi-function device\n");
            // It's a multi-function device, so check remaining functions
            for (function = 1; function < 8; function++) {
                if (pciConfigReadWord(bus, device, function, 0) != 0xFFFF) {
                    checkFunction(bus, device, function);
                }
            }
        }
    } 
}

void checkFunction(uint8_t bus, uint8_t device, uint8_t function)
{
    PciDeviceHeder dv_header = getPciHeader<PciDeviceHeder>(bus, device, function);
    screen.print(GetVendorName(dv_header.vendor_id));
    screen.print(" / ");
    screen.print(GetDeviceName(dv_header.vendor_id, dv_header.device_id));
    screen.print(" / ");
    screen.print(getDeviceClassName(dv_header.class_code));
    screen.print(" / ");
    screen.print(GetSubclassName(dv_header.class_code, dv_header.subclass));
    screen.print(" / ");
    screen.print(GetProgIFName(dv_header.class_code, dv_header.subclass, dv_header.prog_if));
    screen.print("\n");

    //  mass storage controller      |       serial ATA (SATA)    |      AHCI 1.0 device
    if (dv_header.class_code == 0x01 && dv_header.subclass == 0x06 && dv_header.prog_if == 0x01)
    {
        Ahci ahci;
        ahci.init(getPciHeader<PciHeader0>(bus, device, function));
    }
}

void checkAllBuses() 
{
    uint16_t bus;
    uint8_t device;
    // 256 busses with 32 devices each
    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
}


#include "../../headers/PCI.h"


void init() 
{
    // ACPIV1* acpi = (ACPIV1*)getBootInfo<ACPIV1>(14);
    // if (acpi == nullptr) {
    //     write_serial("ACPI not found");
    //     return;
    // }
    // write_serial_var("ACPI found at: ", (uint64_t)acpi);
    // write_serial_var("acpi size", acpi->size);

    // RSDPV1* rsdp = (RSDPV1*)acpi->rsdp;

    // write_serial_var("rsdp found at: ", (uint64_t)rsdp);
    // write_serial_var("rsdp rsdt_address", rsdp->rsdt_address);
    // write_serial_char('\n');


    // RSDT* rsdt = (RSDT*)rsdp->rsdt_address;
    // while (rsdt != 0)
    // {
    //     for (int i = 0; i < 4; i++)
    //     {
    //         write_serial_char(rsdt->header.signature[i]);
    //     }
        
    //     write_serial_char('\n');
    //     rsdt = (RSDT*)rsdt->next_RSDT_ptr;
    // }
    

    // uint16_t vendor, device;
    // uint8_t bus = 0, slot = 0, func;
    // /* Try and read the first configuration register. Since there are no
    //     * vendors that == 0xFFFF, it must be a non-existent device. */
    // if ((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF) {
    //     device = pciConfigReadWord(bus, slot, 0, 2);
    //     write_serial_var("vendor", vendor);
    //     write_serial_var("device", device);
    //     PciDeviceHeder* dv_header = (PciDeviceHeder*)device; 


    //     // for (int i = 0; i < 32; i++)
    //     // {
            
    //     // }
        
    // }
    checkAllBuses();

    // TODO: enumerate all pci devices to see which exist: 


}




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
        PciDeviceHeder dv_header = getDeviceHeader(bus, device, function);
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
        write_serial_char('\n');
    } 
}

void checkFunction(uint8_t bus, uint8_t device, uint8_t function)
{
    PciDeviceHeder dv_header = getDeviceHeader(bus, device, function);
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

PciDeviceHeder getDeviceHeader(uint8_t bus, uint8_t device, uint8_t function)
{
    PciDeviceHeder dv_header = {0};
    uint16_t temp = 0;

    dv_header.vendor_id = pciConfigReadWord(bus, device, function, 0);
    dv_header.device_id = pciConfigReadWord(bus, device, function, 2);
    dv_header.command = pciConfigReadWord(bus, device, function, 4);
    dv_header.status = pciConfigReadWord(bus, device, function,6);

    temp = pciConfigReadWord(bus, device, function, 8);
    dv_header.revision_id = temp & 0x00FF; // low byte
    dv_header.prog_if = (temp & 0xFF00) >> 8; // high byte

    temp = pciConfigReadWord(bus, device, function, 10);
    dv_header.subclass = temp & 0x00FF; // low byte
    dv_header.class_code = (temp & 0xFF00) >> 8; // high byte

    temp = pciConfigReadWord(bus, device, function, 12);
    dv_header.cache_line_size = temp & 0x00FF; // low byte
    dv_header.latency_timer = (temp & 0xFF00) >> 8; // high byte

    temp = pciConfigReadWord(bus, device, function, 14);
    dv_header.header_type = (temp & 0xFF00) >> 8; // high byte
    dv_header.BIST = temp & 0x00FF; // low byte


    return dv_header;
}



// vendorID:8086
// dv_header->device_id:A1
// dv_header->vendor_id:8086
// dv_header->status:8E

// vendorID:8086
// dv_header->device_id:844
// dv_header->vendor_id:3B0C
// dv_header->status:48B
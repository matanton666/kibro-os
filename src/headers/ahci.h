#pragma once

#include "std.h"
#include "serial.h"
#include "pciDescriptors.h"
#include "pageFrameAllocator.h"


struct HBAport // host bus addaptor port
{
    uint32_t command_list_base; // 0x00
    uint32_t command_list_base_upper; // 0x04
    uint32_t fis_base; // 0x08
    uint32_t fis_base_upper; // 0x0C
    uint32_t interrupt_status; // 0x10
    uint32_t interrupt_enable; // 0x14
    uint32_t cmd; // 0x18
    uint32_t reserved0; // 0x1C
    uint32_t task_file_data; // 0x20
    uint32_t signature; // 0x24
    uint32_t sata_status; // 0x28
    uint32_t sata_control; // 0x2C
    uint32_t sata_error; // 0x30
    uint32_t sata_active; // 0x34
    uint32_t command_issue; // 0x38
    uint32_t sata_notification; // 0x3C
    uint32_t fis_based_switch_control; // 0x40
    uint32_t reserved1[11]; // 0x44
    uint32_t vendor_specific[4]; // 0x70
}__attribute__((packed));

struct HBAmemory // host bust addaptor memory
{
    uint32_t host_capabilities;
    uint32_t global_host_control;
    uint32_t interrupt_status;
    uint32_t ports_implemented;
    uint32_t version;
    uint32_t ccc_control;
    uint32_t ccc_ports;
    uint32_t em_location; // enclosure management
    uint32_t em_control; // enclosure management
    uint32_t host_capabilities_ext;
    uint32_t bios_handoff;
    uint8_t reserved[116];
    uint8_t vendor_specific[96];
    HBAport ports[32];
}__attribute__((packed));

// enum PortType
// {
//     None = 0,
//     Sata = 1,
//     SataPI = 2,
//     SataPM = 3,
//     SataPIPM = 4,
//     SataPMPP = 5,
//     SataPMPPSD = 6,
//     SataPMPPSDPI = 7
// };
enum PortType
{
    NONE = 0,
    SATA = 1,
    SEMB = 2,
    PB = 3,
    SATAPI = 4
};


class Ahci
{
private:
    PciHeader0 _pci_base_addr;
    HBAmemory* _abar;

    void probePorts();

    PortType checkPortType(HBAport* port);

public: 

    void init(PciHeader0 pci_base_addr);

};


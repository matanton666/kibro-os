#pragma once

#include "serial.h"
#include "pciDescriptors.h"
#include "virtualMemory.h"


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

struct HBACommandHeader
{
    uint8_t command_fis_length : 5;
    uint8_t atapi : 1;
    uint8_t write : 1;
    uint8_t prefetchable : 1;

    uint8_t reset : 1;
    uint8_t bist : 1;
    uint8_t clear_busy_upon_r_ok : 1;
    uint8_t reserved0 : 1;
    uint8_t port_multiplier : 4;

    uint16_t prdt_length;
    uint32_t prdb_count;
    uint32_t command_table_base;
    uint32_t command_table_base_upper;
    uint32_t reserved1[4];
};



enum PortType
{
    NONE = 0,
    SATA = 1,
    SEMB = 2,
    PB = 3,
    SATAPI = 4
};

enum HBA_PxCMD
{
    CR = 0x8000,
    FRE = 0x0010,
    ST = 0x0001,
    FR = 0x4000
};


class Port
{
public:
    HBAport* _port;
    PortType _type;
    uint8_t* _buffer;
    uint8_t _port_num;

    void configure();
    void startCmd();
    void stopCmd();
}__attribute__((packed));


class Ahci
{
private:
    PciHeader0 _pci_base_addr;
    HBAmemory* _abar;
    Port _ports[32] = {0};
    uint8_t _port_count = 0;

    void probePorts();

    PortType checkPortType(HBAport* port);

public: 

    void init(PciHeader0 pci_base_addr);

};


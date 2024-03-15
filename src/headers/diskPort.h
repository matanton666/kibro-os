#pragma once

#include "std.h"
#include "serial.h"
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

struct HBAPRDTEntry
{
    uint32_t database_address;
    uint32_t database_address_upper;
    uint32_t reserved0;

    uint32_t byte_count : 22;
    uint32_t reserved1 : 9;
    uint32_t interrupt_on_completion : 1;
}__attribute__((packed));

struct HBACommandTable
{
    uint8_t command_fis[64];
    uint8_t atapi_command[16];
    uint8_t reserved[48];
    HBAPRDTEntry prdt_entry[];

}__attribute__((packed));



struct FIS_REG_H2D // frame information structure register hardware to device
{
    uint8_t fis_type;

    uint8_t port_multiplyer : 4;
    uint8_t reserved0 : 3;
    uint8_t command_control : 1;

    uint8_t command;
    uint8_t feature_low;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device_register;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t feature_high;

    uint8_t count_low;
    uint8_t count_high;
    uint8_t iso_command_completion;
    uint8_t control;

    uint8_t rsv1[4];
}__attribute__((packed));



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

enum FIS_TYPE
{
    REG_H2D = 0x27,
    REG_D2H = 0x34,
    DMA_ACT = 0x39,
    DMA_SETUP = 0x41,
    DATA = 0x46,
    BIST = 0x58,
    PIO_SETUP = 0x5F,
    DEV_BITS = 0xA1,
};

enum ATA_STATUS
{
    ATA_STATUS_BSY = 0x80,
    ATA_STATUS_DRQ = 0x08,
    ATA_STATUS_ERR = 0x01,
    ATA_CMD_READ_DMA_EX = 0x25,
    ATA_CMD_WRITE_DMA_EX = 0x35,
};

enum HBA_PxIS
{
    TFES = (1 << 30),
};


class DiskPort
{
private:
    HBAport* _port;
    PortType _type;
    uint8_t _port_num;
    // uint32_t _free_space;
    // uint32_t _used_space;

    bool access(uint64_t sector, uint32_t sector_count, uint8_t* buffer, bool is_write);
    void startCmd();
    void stopCmd();

    // read from sector - amount of sectors 
    bool readSec(uint64_t sector, uint32_t sector_count, uint8_t* buffer);

    // writes to sector - amount of sectors
    bool writeSec(uint64_t sector, uint32_t sector_count, uint8_t* buffer);
    void configure();


public:

    void init(HBAport* port, PortType type, uint8_t port_num);

    // read from address - amount of bytes
    bool read(uint32_t address, uint32_t size, uint8_t* buffer);

    // writes to address - amount of bytes
    bool write(uint32_t address, uint32_t size, uint8_t* buffer);

    PortType getType() { return _type; }
}__attribute__((packed));


#include "../../headers/ahci.h"
#include "virtualMemory.h"

extern MemoryManager::PagingSystem kernelPaging;

void Ahci::init(PciHeader0 pci_base_addr)
{
    _pci_base_addr = pci_base_addr;
    _abar = (HBAmemory*)(pci_base_addr.bar5 & 0xFFFFFFF0);

    // kernelPaging.identityPaging((uintptr_t)_abar, (uintptr_t)_abar + sizeof(HBAmemory)); // identity map 
    write_serial_var("abar", (uintptr_t)_abar);
    // phys_mem.lockPages((unsigned char*)_abar, 1); // lock the page

    write_serial("probing ports");
    probePorts();
    write_serial("ahci init");
}



void Ahci::probePorts()
{
    uint32_t ports_implemented = _abar->ports_implemented;
    for (int i = 0; i < 32; i++)
    {
        if (ports_implemented & (1 << i))
        {
            PortType port_type = checkPortType(&_abar->ports[i]);
            if (port_type == PortType::SATA)
            {
                write_serial("found a sata drive");
            }
        }
    }
}



PortType Ahci::checkPortType(HBAport* port)
{
    uint32_t sata_status = port->sata_status;

    uint8_t interface_power_management = (sata_status >> 8) & 0b111; // TODO: might need to be 0b111 and not 0x0F
    uint8_t device_detection = sata_status & 0b111;

    if (device_detection != 3) // no device
    {
        return PortType::NONE;
    }

    if (interface_power_management != 1)
    {
        return PortType::NONE;
    }

    switch (port->signature)
    {
    case 0x00000101:
        return PortType::SATA;
    case 0xEB140101:
        return PortType::SATAPI;
    case 0xC33C0101:
        return PortType::SEMB;
    case 0x96690101:
        return PortType::PB;
    default:
        return PortType::NONE;
    }
}
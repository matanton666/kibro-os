#include "../../headers/ahci.h"

// extern DiskPort disk;

void Ahci::init(PciHeader0 pci_base_addr)
{
    _pci_base_addr = pci_base_addr;
    // memset(_ports, 0, sizeof(DiskPort)*32);

    _abar = (HBAmemory*)(pci_base_addr.bar5 & 0xFFFFFFF0);
    kernelPaging.identityPaging((uintptr_t)_abar, (uintptr_t)_abar + sizeof(HBAmemory)); // identity map 

    probePorts();
    for (int i = 0; i < _port_count; i++)
    {
        DiskPort disk = _ports[i]; // set disk to first sata port
        disk.configure();
        uint8_t* _buffer = (uint8_t*)kernelPaging.getAllocator()->mallocAligned(256, 256);
        // memset(port._buffer, 65, 100);
        // port.write(0, 4, port._buffer);
        disk.read(0, 4, (uint8_t*)_buffer);

        // for (int j = 0; j < 256 / sizeof(uint8_t); j++)
        // {
        //     screen.print((char)_buffer[j]);
        // }
    }
    
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
                write_serial("found sata");
                // _ports[_port_count] = (DiskPort*)kernelPaging.getAllocator()->mallocAligned(sizeof(DiskPort), 256);
                _ports[_port_count].init(&_abar->ports[i], port_type, _port_count);
                _port_count++;
            }
        }
    }
}



PortType Ahci::checkPortType(HBAport* port)
{
    uint32_t sata_status = port->sata_status;

    uint8_t interface_power_management = (sata_status >> 8) & 0b111;
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


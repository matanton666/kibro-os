#pragma once

#include "diskPort.h"
#include "pciDescriptors.h"


class Ahci
{
private:
    PciHeader0 _pci_base_addr;
    HBAmemory* _abar;
    // DiskPort _ports[32];
    // uint8_t _port_count = 0;
    DiskPort _port;

    void probePorts();

    PortType checkPortType(HBAport* port);

public: 

    void init(PciHeader0 pci_base_addr);
};


// extern DiskPort disk;

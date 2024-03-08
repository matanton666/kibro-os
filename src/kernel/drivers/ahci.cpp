#include "../../headers/ahci.h"

void Port::configure()
{
    stopCmd();

    void* buffer = (void*)kernelPaging.getAllocator()->mallocAligned(PAGE_SIZE, KIB1);// 1-K aligned
    _port->command_list_base = (uint32_t)(uint64_t)buffer;
    _port->command_list_base_upper = (uint32_t)((uint64_t)buffer >> 32);
    memset((void*)(_port->command_list_base), 0, 1024);

    void* fis_base = (void*)kernelPaging.getAllocator()->mallocAligned(PAGE_SIZE, 256);
    _port->fis_base = (uint32_t)(uint64_t)fis_base;
    _port->fis_base_upper = (uint32_t)((uint64_t)fis_base >> 32);
    memset(fis_base, 0, 256);

    HBACommandHeader* command_header = (HBACommandHeader*)((uint64_t)_port->command_list_base + ((uint64_t)_port->command_list_base_upper << 32));

    for (int i = 0; i < 32; i++)
    {
        command_header[i].prdt_length = 8; // 8 prdt entries per command table

        void* command_table = (void*)kernelPaging.getAllocator()->mallocAligned(PAGE_SIZE, 256);
        uint64_t address = (uint64_t)command_table + (i << 8);
        command_header[i].command_table_base = (uint32_t)address;
        // command_header[i].command_table_base_upper = (uint32_t)(address >> 32);
        command_header[i].command_table_base_upper = (uint32_t)((uint64_t)address >> 32);
        memset(command_table, 0, 256);
    }
    
    startCmd();
}

void Port::startCmd()
{
    while (_port->cmd & HBA_PxCMD::CR); // cant start command if it is already running

    _port->cmd |= HBA_PxCMD::FRE; // enable start bit
    _port->cmd |= HBA_PxCMD::ST; // enable fre bit
}

void Port::stopCmd()
{
    _port->cmd &= ~HBA_PxCMD::ST; // disable fre bit
    _port->cmd &= ~HBA_PxCMD::FRE; // disable start bit

    while (true)
    {
        if (_port->cmd & HBA_PxCMD::FR || _port->cmd & HBA_PxCMD::CR) // command is running
        {
            continue;
        }
        else // command is finished
        {
            break;
        }
    }
}

void Ahci::init(PciHeader0 pci_base_addr)
{
    _pci_base_addr = pci_base_addr;
    _abar = (HBAmemory*)(pci_base_addr.bar5 & 0xFFFFFFF0);

    kernelPaging.identityPaging((uintptr_t)_abar, (uintptr_t)_abar + sizeof(HBAmemory)); // identity map 
    write_serial_var("abar", (uintptr_t)_abar);

    probePorts();
    write_serial_var("port count", _port_count);
    for (int i = 0; i < _port_count; i++)
    {
        Port port = _ports[i];
        port.configure();
        write_serial_var("port num", port._port_num);
        write_serial_var("type", (uint32_t)port._type);
        write_serial_var("port", (uintptr_t)port._port);
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
                _ports[_port_count]._type = port_type;
                _ports[_port_count]._port = &_abar->ports[i];
                _ports[_port_count]._port_num = _port_count;
                _port_count++;
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

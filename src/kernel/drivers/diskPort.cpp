#include "../../headers/diskPort.h"



void DiskPort::configure()
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

void DiskPort::startCmd()
{
    while (_port->cmd & HBA_PxCMD::CR); // cant start command if it is already running

    _port->cmd |= HBA_PxCMD::FRE; // enable start bit
    _port->cmd |= HBA_PxCMD::ST; // enable fre bit
}

void DiskPort::stopCmd()
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



void DiskPort::init(HBAport* port, PortType type, uint8_t port_num)
{
    _port = port;
    _type = type;
    _port_num = port_num;
    configure();
}

bool DiskPort::read_sec(uint64_t sector, uint32_t sector_count, uint8_t* buffer)
{
    return access(sector, sector_count, buffer, false);
}

bool DiskPort::write_sec(uint64_t sector, uint32_t sector_count, uint8_t* buffer)
{
    return access(sector, sector_count, buffer, true);
}


bool DiskPort::read(uint32_t address, uint32_t size, uint8_t* buffer)
{
    // translate address to sector
    uint64_t start_sector = address / 512;
    uint32_t sector_count = (size / 512) + ((size % 512) ? 1 : 0);

    // Allocate a temporary buffer to read sectors
    uint8_t* temp_buffer = (uint8_t*)kernelPaging.getAllocator()->malloc(sector_count * 512);

    // Read sectors into temporary buffer
    if (!read_sec(start_sector, sector_count, temp_buffer)) {
        kernelPaging.getAllocator()->free(temp_buffer);
        return false;
    }

    // Calculate offset into the first sector
    uint32_t offset = address % 512;

    // Copy the requested size into the buffer
    memcpy(buffer, temp_buffer + offset, size);

    kernelPaging.getAllocator()->free(temp_buffer);
    return true;
}


bool DiskPort::write(uint32_t address, uint32_t size, uint8_t* buffer)
{
    // TODO: need to test this on differents sizes that exceed 512
    // translate address to sector
    uint64_t sector = address / 512;
    uint32_t sector_count = (size / 512) + ((size % 512) ? 1 : 0);

    // Allocate a temporary buffer to hold the data to be written
    uint8_t* temp_buffer = (uint8_t*)kernelPaging.getAllocator()->malloc(sector_count * 512);


    // If the address is not a multiple of the sector size, we need to read the first sector first,
    // modify the part we want, and then write it back.
    if (address % 512 != 0) {
        if (!read_sec(sector, 1, temp_buffer)) {
            kernelPaging.getAllocator()->free(temp_buffer);
            return false;
        }
    }

    // Copy the data to the temporary buffer
    memcpy(temp_buffer + (address % 512), buffer, size);

    // If the size is not a multiple of the sector size, we need to read the last sector first,
    // modify the part we want, and then write it back.
    if (size % 512 != 0 && size > 512) {
        if (!read_sec(sector + sector_count - 1, 1, temp_buffer + 512 * (sector_count - 1))) {
            kernelPaging.getAllocator()->free(temp_buffer);
            return false;
        }
    }

    // Write the sectors from the temporary buffer
    bool result = write_sec(sector, sector_count, temp_buffer);

    kernelPaging.getAllocator()->free(temp_buffer);
    return result;
}

bool DiskPort::access(uint64_t sector, uint32_t sector_count, uint8_t* buffer, bool is_write)
{
    uint32_t sector_low = (uint32_t)sector;
    uint32_t sector_high = (uint32_t)(sector >> 32);

    uint32_t spin = 0;
    while ((_port->task_file_data & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000) // wait for device to be ready
    {
        spin++; 
    }
    if (spin == 1000000)
    {
        return false;
    }

    _port->interrupt_status = (uint32_t)-1; // clear pending interrupt status
    HBACommandHeader* command_header = (HBACommandHeader*)_port->command_list_base;
    command_header->command_fis_length = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size

    command_header->write = is_write ? 1 : 0; // indicate read or write
    command_header->prdt_length = 1; // only 1 prdt entry

    HBACommandTable* command_table = (HBACommandTable*)(command_header->command_table_base);
    memset(command_table, 0, sizeof(HBACommandTable) + (command_header->prdt_length - 1) * sizeof(HBAPRDTEntry));

    command_table->prdt_entry[0].database_address = (uint32_t)(uint64_t)buffer;
    command_table->prdt_entry[0].database_address_upper = (uint32_t)((uint64_t)buffer >> 32);
    command_table->prdt_entry[0].byte_count = (sector_count<<9) - 1; // 512 bytes per sector
    command_table->prdt_entry[0].interrupt_on_completion = 1;

    FIS_REG_H2D* fis = (FIS_REG_H2D*)(&command_table->command_fis);
    fis->fis_type = FIS_TYPE::REG_H2D;
    fis->command_control = 1; // is command
    fis->command = is_write ? ATA_STATUS::ATA_CMD_WRITE_DMA_EX : ATA_STATUS::ATA_CMD_READ_DMA_EX; // read or write command

    fis->lba0 = (uint8_t)sector_low;
    fis->lba1 = (uint8_t)(sector_low >> 8);
    fis->lba2 = (uint8_t)(sector_low >> 16);
    fis->lba3 = (uint8_t)sector_high;
    fis->lba4 = (uint8_t)(sector_high >> 8);
    fis->lba5 = (uint8_t)(sector_high >> 16);
    fis->device_register = 1<<6; // LBA mode

    fis->count_low = sector_count & 0xFF;
    fis->count_high = (sector_count >> 8) & 0xFF;


    _port->command_issue = 1; // issue command

    while (true)
    {
        if (_port->command_issue == 0)
        {
            break;
        }
        if (_port->interrupt_status & HBA_PxIS::TFES) // device error
        {
            return false;
        }
    }

    return true;
}


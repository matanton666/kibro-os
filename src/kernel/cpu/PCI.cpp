#include "../../headers/PCI.h"


void init() 
{
    ACPIV1* acpi = (ACPIV1*)getBootInfo<ACPIV1>(14);
    if (acpi == nullptr) {
        write_serial("ACPI not found");
        return;
    }
    write_serial_var("ACPI found at: ", (uint64_t)acpi);

    write_serial_char((uint8_t)acpi->rsdp[0]);
    write_serial_char((uint8_t)acpi->rsdp[1]);
    write_serial_char((uint8_t)acpi->rsdp[2]);
    write_serial_char((uint8_t)acpi->rsdp[3]);
    write_serial_char((uint8_t)acpi->rsdp[4]);
    write_serial_char((uint8_t)acpi->rsdp[5]);
    write_serial_char((uint8_t)acpi->rsdp[6]);
    write_serial_char((uint8_t)acpi->rsdp[7]);
    write_serial_char('\n');

    // TODO: continue here
    // TODO: check if rsdp v1 can work and find the specification for it. if not then find out how to get rsdpv2 to work
}
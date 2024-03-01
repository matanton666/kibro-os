#include "../../headers/PCI.h"


void init() 
{
    ACPIV1* acpi = (ACPIV1*)getBootInfo<ACPIV1>(14);
    if (acpi == nullptr) {
        write_serial("ACPI not found");
        return;
    }
    write_serial_var("ACPI found at: ", (uint64_t)acpi);
    write_serial_var("acpi size", acpi->size);

    RSDPV1* rsdp = (RSDPV1*)acpi->rsdp;

    write_serial_var("rsdp found at: ", (uint64_t)rsdp);
    write_serial_var("rsdp rsdt_address", rsdp->rsdt_address);
    write_serial_char('\n');


    RSDT* rsdt = (RSDT*)rsdp->rsdt_address;
    while (rsdt != 0)
    {
        // write_serial_var("rsdt found at: ", (uint64_t)rsdt);
        // write_serial_var("rsdt signature", (char*)rsdt->header.signature);
        // write_serial_var("rsdt length", rsdt->header.length);
        // write_serial_var("rsdt revision", rsdt->header.revision);
        // write_serial_var("rsdt checksum", rsdt->header.checksum);
        // write_serial_var("rsdt oem_id", (char*)rsdt->header.oem_id);
        // write_serial_var("rsdt oem_table_id", (char*)rsdt->header.oem_table_id);
        // write_serial_var("rsdt oem_revision", rsdt->header.oem_revision);
        // write_serial_var("rsdt creator_id", rsdt->header.creator_id);
        // write_serial_var("rsdt creator_revision", rsdt->header.creator_revision);
        
        // write_serial_var("rsdt next_RSDT_ptr", rsdt->next_RSDT_ptr);
        for (int i = 0; i < 4; i++)
        {
            write_serial_char(rsdt->header.signature[i]);
        }
        
        write_serial_char('\n');
        rsdt = (RSDT*)rsdt->next_RSDT_ptr;
    }
    
    // STDHeader* xstd = (STDHeader*)rsdp->xsdt_address;

    // TODO: continue here
    // TODO: check if rsdp v1 can work and find the specification for it. if not then find out how to get rsdpv2 to work
}
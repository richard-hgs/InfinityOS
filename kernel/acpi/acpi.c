
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "farptr.h"
#include "kstring.h"
#include "io.h"
#include "timer.h"
#include "memutils.h"
#include "print.h"
#include "checksum.h"
#include "array.h"
#include "acpi.h"

#define EBDA_BEGIN_ADDR 0x000E0000
#define EBDA_END_ADDR 0x000FFFFF

#define ACPI_SIG_RSDP "RSD PTR "
#define ACPI_SIG_APIC "APIC"
#define ACPI_SIG_FACP "FACP"

int acpi_sum_bytes(unsigned char* p, int size) {
   int count = 0;
   for (int i=0; i<size; i++) {
       count += p[i];
   }
   return count;
}

int acpi_get_n_byte(int val, int nbyte) {
    return ((val >> nbyte * 8) & 0xFF);
}

bool acpi_checksum_sdt_header(ACPISDTHeader_t* tableHeader, int length) {
    unsigned char sum = 0;

    kprintf("tableHeader -> length: %i\n", length);
    for (int i=0; i<length; i++) {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}

/**
 * Locate the rsdp or return a errorMsg
 * @param revision  1=ACPI_1.0  2=ACPI_2.0-ACPI_6.1
 * @param rsdp      Se revision == 1
 * @param rsdp20    Se revision == 2
 * @param errorMsg  NULL=Success, !=NULL=ErrorMsg
 */
void acpi_locate_rsdp(int* revision, RSDPDescriptor_t* rsdp, RSDPDescriptor20_t* rsdp20, char** errorMsg) {
    bool rsdpFound = false;
    void* rsdpAddress = 0;
    char currentSignature[9];
    char currByte[1];
    for (int i=EBDA_BEGIN_ADDR; i<EBDA_END_ADDR; i++) {
        int currentOffset = 7;
        if (i<8) {
            // Increment the offset
            currentOffset = i;
        } else {
            // Offset already in his max value
            // Copy the bytes back 1 position
            for (int x=1; x<8; x++) {
                currentSignature[x-1] = currentSignature[x];
            }
        }
        memcpy(currByte, (void*) i, 1);
        
        currentSignature[currentOffset] = currByte[0];
        currentSignature[currentOffset + 1] = '\0';

        int strcmpResult = kstrcmp(currentSignature, ACPI_SIG_RSDP);
        if (strcmpResult == 0) {
            rsdpFound = true;
            rsdpAddress = (void*) (i - 7);
            break;
        }
    }

    if (!rsdpFound) {
        *errorMsg = "RSDP   - not found";
    } else {
        memcpy(rsdp, rsdpAddress, sizeof(RSDPDescriptor_t));

        kprintf("RSDP   - found at         : 0x%p\n", rsdpAddress);
        kprintf("RSDP   - signature        : %s\n", rsdp->Signature);
        kprintf("RSDP   - checksum         : 0x%x\n", rsdp->Checksum);
        kprintf("RSDP   - oemid            : %s\n", rsdp->OEMID);
        kprintf("RSDP   - revision         : %i\n", rsdp->Revision);
        kprintf("RSDP   - rsdtAddress      : %x\n", rsdp->RsdtAddress);

        // Validate the rsdp
        if (rsdp->Revision == 0) {
            // Then acpi 1.0
            int sum = acpi_sum_bytes((unsigned char*) rsdp, sizeof(*rsdp));
            int lowest = acpi_get_n_byte(sum, 0);
            if (lowest != 0) {
                // Checksum is invalid
                char mError[100];
                snprintf(mError, 100, "RSDP   - rsdp checksum is invalid - sum: %x - firstByte: %x", sum, lowest);
                *errorMsg = mError;
            } 
            // else {
            //     *revision = rsdp->Revision;
            // }
        } else {
            // Then acpi 2.0
            memcpy(rsdp20, rsdpAddress, sizeof(RSDPDescriptor20_t));
            kprintf("RSDP20 - length           : %i\n", rsdp20->Length);
            kprintf("RSDP20 - xsdtAddress      : 0x%p\n", rsdp20->XsdtAddress);
            kprintf("RSDP20 - extendedChecksum : 0x%x\n", rsdp20->ExtendedChecksum);

            int sum1 = acpi_sum_bytes((unsigned char *) rsdp, sizeof(*rsdp));
            int lowest1 = acpi_get_n_byte(sum1, 0);

            int sum2 = acpi_sum_bytes((unsigned char *) &(rsdp20->Length), sizeof(rsdp20->Length)) + 
                       acpi_sum_bytes((unsigned char *) &(rsdp20->XsdtAddress), sizeof(rsdp20->XsdtAddress)) +
                       acpi_sum_bytes((unsigned char *) &(rsdp20->ExtendedChecksum), sizeof(rsdp20->ExtendedChecksum)) + 
                       acpi_sum_bytes((unsigned char *) &(rsdp20->Reserved), sizeof(rsdp20->Reserved));
            int lowest2 = acpi_get_n_byte(sum2, 0);

            if (lowest1 != 0 || lowest2 != 0) {
                // Checksum is invalid
                char mError[160];
                snprintf(mError, 160, "RSDP20 - checksum is invalid - sum1: %i - firstByte1: %i - sum2: %i - firstByte2: %i\n", sum1, lowest1, sum2, lowest2);
                *errorMsg = mError;
            } 
            // else {
            //     *revision = rsdp->Revision;
            // }
        }
    }
}

void hexDump (
    const char * desc,
    const void * addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.

    if (perLine < 4 || perLine > 64) { 
        perLine = 16;
    }

    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL) kprintf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        kprintf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        kprintf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.

            if (i != 0) kprintf ("  %s\n", buff);

            // Output the offset of current line.

            kprintf ("  %04X ", i & 0xFFFF);
        }

        // Now the hex code for the specific character.

        kprintf (" %02X", pc[i] & 0xFF);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        kprintf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    kprintf ("  %s\n", buff);
}

void acpi_dump_definition_block(uint32_t startOffset, uint32_t size) {
    // ACPI PAGE 1112;
    hexDump(0, (void*) startOffset, (int) size, 16);
    // int leading = 0;
    // for (size_t offset = startOffset; offset < startOffset + size; offset++) {
    //     char amlChar = *((char*) offset);
    //     int amlInt = (int) amlChar;

    //     bool incrementLeading = false;

    //     dskprintf("%x", amlInt & 0xFF);

    //     // if (amlInt == 0x10) {
    //     //     kprintf("%s", "DefinitionBlock\n");
    //     //     // incrementLeading = true;
    //     //     // leading += 2;
    //     // } else if (amlInt == 0x08) {
    //     //     kprintf("%s", "NameOp\n");
    //     // } else {

    //     // if (amlInt == 0) {
    //     //     kprintf(".");
    //     // } else {
    //     //     kprintf("%c", amlChar);
    //     // }
    // }
}

void acpi_locate_rsdt() {
    kprintf("Locating RSDP\n");
	int rsdpRevision = 0;
	RSDPDescriptor_t rsdp;
	RSDPDescriptor20_t rsdp20;
	char* errorMsg = NULL;
	acpi_locate_rsdp(&rsdpRevision, &rsdp, &rsdp20, &errorMsg);
	if (errorMsg) {
		kprintf("%s\n", errorMsg);
	} else {
        kprintf("\nLocating RSDT\n");
        
        RSDT_t rootRsdt;
        memcpy(&rootRsdt, (void*) rsdp.RsdtAddress, sizeof(RSDT_t));

        if (acpi_checksum_sdt_header(&(rootRsdt.h), sizeof(rootRsdt.h))) {
            kprintf("RSDT - rsdt checksum is invalid\n");
        } else {
            kprintf("RSDT - rsdt found at   : 0x%x\n", rsdp.RsdtAddress);
            kprintf("RSDT - signature       : %s\n", rootRsdt.h.Signature);
            kprintf("RSDT - length          : %i\n", rootRsdt.h.Length);
            kprintf("RSDT - revision        : %i\n", rootRsdt.h.Revision);
            kprintf("RSDT - checksum        : 0x%x\n", rootRsdt.h.Checksum);
            kprintf("RSDT - oemid           : %s\n", rootRsdt.h.OEMID);
            kprintf("RSDT - oemtableid      : %s\n", rootRsdt.h.OEMTableID);
            kprintf("RSDT - oemrevision     : %u\n", rootRsdt.h.OEMRevision);
            kprintf("RSDT - creatorid       : %u\n", rootRsdt.h.CreatorID);
            kprintf("RSDT - creatorrevision : %u\n", rootRsdt.h.CreatorRevision);

            // Now locate the tables
            for (int i=0; i<4; i++) {
                ACPISDTHeader_t* h = (ACPISDTHeader_t*) rootRsdt.PointerToOtherSDT[i];
                // kprintf("signature: %s | ", h->Signature);
                if (!kstrncmp(h->Signature, ACPI_SIG_FACP, 4)) {
                    FADT_t* fadt = (FADT_t*) h;
                    if (acpi_checksum_sdt_header(h, sizeof(h))) {
                        kprintf("FADT - fadt checksum is invalid\n");
                    } else {
                        kprintf("\nFADT - fadt found at       : 0x%x\n", fadt);
                        kprintf("FADT - signature           : %s\n", fadt->h.Signature);
                        kprintf("FADT - length              : %i\n", fadt->h.Length);
                        kprintf("FADT - revision            : %i\n", fadt->h.Revision);
                        kprintf("FADT - checksum            : 0x%x\n", fadt->h.Checksum);
                        kprintf("FADT - oemid               : %s\n", fadt->h.OEMID);
                        kprintf("FADT - oemtableid          : %s\n", fadt->h.OEMTableID);
                        kprintf("FADT - oemrevision         : %u\n", fadt->h.OEMRevision);
                        kprintf("FADT - creatorid           : %u\n", fadt->h.CreatorID);
                        kprintf("FADT - creatorrevision     : %u\n", fadt->h.CreatorRevision);
                        kprintf("FADT - FIRMWARE_CTRL       : %x\n", fadt->FIRMWARE_CTRL);
                        kprintf("FADT - DSDT                : %x\n", fadt->Dsdt);
                        kprintf("FADT - Reserved            : %x\n", fadt->Reserved);
                        kprintf("FADT - Preferred_PM_Profile: %i\n", fadt->Preferred_PM_Profile);
                        kprintf("FADT - SCI_INT             : %i\n", fadt->SCI_INT);
                        kprintf("FADT - SMI_CMD             : %x\n", fadt->SMI_CMD);
                        kprintf("FADT - ACPI_ENABLE         : %i\n", fadt->ACPI_ENABLE);
                        kprintf("FADT - ACPI_DISABLE        : %i\n", fadt->ACPI_DISABLE);
                        kprintf("FADT - S4BIOS_REQ          : %i\n", fadt->S4BIOS_REQ);
                        kprintf("FADT - PSTATE_CNT          : %i\n", fadt->PSTATE_CNT);
                        kprintf("FADT - PM1a_EVT_BLK        : %x\n", fadt->PM1a_EVT_BLK);
                        kprintf("FADT - PM1b_EVT_BLK        : %x\n", fadt->PM1b_EVT_BLK);
                        kprintf("FADT - PM1a_CNT_BLK        : %x\n", fadt->PM1a_CNT_BLK);
                        kprintf("FADT - PM1b_CNT_BLK        : %x\n", fadt->PM1b_CNT_BLK);
                        kprintf("FADT - PM2_CNT_BLK         : %x\n", fadt->PM2_CNT_BLK);
                        kprintf("FADT - PM_TMR_BLK          : %x\n", fadt->PM_TMR_BLK);
                        kprintf("FADT - GPE0_BLK            : %x\n", fadt->GPE0_BLK);
                        kprintf("FADT - GPE1_BLK            : %x\n", fadt->GPE1_BLK);
                        kprintf("FADT - PM1_EVT_LEN         : %i\n", fadt->PM1_EVT_LEN);
                        kprintf("FADT - PM1_CNT_LEN         : %i\n", fadt->PM1_CNT_LEN);
                        kprintf("FADT - PM2_CNT_LEN         : %i\n", fadt->PM2_CNT_LEN);
                        kprintf("FADT - PM_TMR_LEN          : %i\n", fadt->PM_TMR_LEN);
                        kprintf("FADT - GPE0_BLK_LEN        : %i\n", fadt->GPE0_BLK_LEN);
                        kprintf("FADT - GPE1_BLK_LEN        : %i\n", fadt->GPE1_BLK_LEN);
                        kprintf("FADT - GPE1_BASE           : %i\n", fadt->GPE1_BASE);
                        kprintf("FADT - CST_CNT             : %i\n", fadt->CST_CNT);
                        kprintf("FADT - P_LVL2_LAT          : %i\n", fadt->P_LVL2_LAT);
                        kprintf("FADT - P_LVL3_LAT          : %i\n", fadt->P_LVL3_LAT);
                        kprintf("FADT - FLUSH_SIZE          : %i\n", fadt->FLUSH_SIZE);
                        kprintf("FADT - FLUSH_STRIDE        : %i\n", fadt->FLUSH_STRIDE);

                        kprintf("\nLocating DSDT\n");
                        DSDT_t* dsdt = (DSDT_t*) fadt->Dsdt;
                        // ACPISDTHeader_t* dsdtHeader = (ACPISDTHeader_t*) fadt->Dsdt;
                        // if (acpi_checksum_sdt_header(&(dsdt->h), sizeof(dsdt->h))) {
                        kprintf("DSDT - dsdt found at   : 0x%x\n", fadt->Dsdt);
                        kprintf("DSDT - signature       : %s\n", dsdt->h.Signature);
                        kprintf("DSDT - length          : %i\n", dsdt->h.Length);
                        kprintf("DSDT - revision        : %i\n", dsdt->h.Revision);
                        kprintf("DSDT - checksum        : 0x%x\n", dsdt->h.Checksum);
                        kprintf("DSDT - oemid           : %s\n", dsdt->h.OEMID);
                        kprintf("DSDT - oemtableid      : %s\n", dsdt->h.OEMTableID);
                        kprintf("DSDT - oemrevision     : %u\n", dsdt->h.OEMRevision);
                        kprintf("DSDT - creatorid       : %u\n", dsdt->h.CreatorID);
                        kprintf("DSDT - creatorrevision : %u\n", dsdt->h.CreatorRevision);

                        acpi_dump_definition_block(((uint32_t) dsdt) + sizeof(dsdt->h), 6009 - sizeof(dsdt->h));

                        // kprintf("dsdt memory 0x%x", (uint32_t) dsdt);
                        // for (uint32_t offset=((uint32_t) dsdt); offset < ((uint32_t) dsdt) + 100; offset++) {
                        //     kprintf("%c", *((char*) offset));
                        // }

                        // memprintAsStr(((uint32_t) dsdt) + sizeof(dsdt->h), 6009, 80);
                    }
                }
                // if (!kstrncmp(h->Signature, APIC_SIGNATURE, 4)) {
                //     // apic found read structure
                //     MADT_t* madt = (MADT_t*) h;
                //     // Should load a differentiated definition block first acpi 6.0 page 320
                //     kprintf("\nMADT       - LocalInterruptControllerAddr: 0x%x\n", madt->LocalInterruptControllerAddr);
                //     kprintf("MADT       - Flags: %i\n", madt->Flags);
                //     kprintf("MADT_LAPIC - Type: %i\n", madt->madtLapic.Type);
                //     kprintf("MADT_LAPIC - Length: %i\n", madt->madtLapic.Length);
                //     kprintf("MADT_LAPIC - CpuUid: %i\n", madt->madtLapic.CpuUid);
                //     kprintf("MADT_LAPIC - ApicId: %i\n", madt->madtLapic.ApicId);
                //     kprintf("MADT_LAPIC - Flags: %i\n", madt->madtLapic.Flags);
                // }
            }
        }

        // unsigned char* p = (unsigned char*) &rsdt.h;
        // uint32_t rsdtChecksum = checksum_8bit_mod256(p, sizeof(ACPISDTHeader_t), 0, 0);
        // kprintf("RSDT - sizeof: %i", ((rsdt.h.Length - sizeof(rsdt.h)) / 4));

        // if (rsdpRevision == 2) {
        //     // Locate xsdt
        //     // memcpy(&xsdt, (void*) rsdp20.XsdtAddress, sizeof(XSDT_t));
        //     // if (!acpi_checksum_sdt_header(&(xsdt.h))) {
        //     //     kprintf("XSDT - xsdt checksum is invalid\n");
        //     // } else {
        //     //     kprintf("XSDT - rsdt found at   : 0x%x\n", rsdp20.XsdtAddress);
        //     //     kprintf("XSDT - signature       : %s\n", xsdt.h.Signature);
        //     //     kprintf("XSDT - length          : %i\n", xsdt.h.Length);
        //     //     kprintf("XSDT - revision        : %i\n", xsdt.h.Revision);
        //     //     kprintf("XSDT - checksum        : 0x%x\n", xsdt.h.Checksum);
        //     //     kprintf("XSDT - oemid           : %s\n", xsdt.h.OEMID);
        //     //     kprintf("XSDT - oemtableid      : %s\n", xsdt.h.OEMTableID);
        //     //     kprintf("XSDT - oemrevision     : %u\n", xsdt.h.OEMRevision);
        //     //     kprintf("XSDT - creatorid       : %u\n", xsdt.h.CreatorID);
        //     //     kprintf("XSDT - creatorrevision : %u\n", xsdt.h.CreatorRevision);
        //     // }
        // } else {
        //     // Locate rsdt
        // }
    }
}

void acpi_dump() {
    // print start string
}
/*
 * helper.h - Header file for acpi utilities
 *
 * Author: Infinity Technology
 * Date  : 07/04/2020
 *
 *********************************************************************
 */
#ifndef _ACPI_H_
#define _ACPI_H_
#pragma once

/**
 * RSDPDescriptor
 *  __________________________________________________________________________________________________________________
 * |     WHAT     |   SIZE  |  OFFSET  |                                DESCRIPTION                                       |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |  SIGNATURE   | 8 Bytes |    0     | "RSD PTR " (Notice that this signature must contain a trailing blank character.) |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |   CHECKSUM   | 1 Byte  |    8     | This is the checksum of the fields defined in the ACPI 1.0 specification.        |
 * |              |         |          | This includes only the first 20 bytes of this table, bytes 0-19, including the   |
 * |              |         |          | checksum field. These bytes must sum to zero.                                    |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |    OEMID     | 6 Bytes |    9     | An OEM-Supplied string that identifies the OEM.                                  |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |   REVISION   | 1 Byte  |    15    | The revision of this structure. Larger revision numbers are backward compatible  |
 * |              |         |          | to lower revision numbers. If this field contains 0 then is acpi 1.0             |
 * |              |         |          | otherwise is acpi 2.0-6.1                                                        |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * | RSDT_ADDRESS | 4 Bytes |    16    | 32 bit physical address of the RSDT.                                             |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |   LENGTH     | 4 Bytes |    20    | The length of the table, in byes including the header, starting from offset 0.   |
 * |              |         |          | This field is used to record the size of the entire table.                       |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * | XSDT_ADDRESS | 4 Bytes |    24    | 64 bit physical address of the XSDT                                              |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * | EXT_CHECKSUM | 1 Byte  |    32    | This is a checksum of the entire table, including both checksum fields.          |
 * |--------------|---------|----------|----------------------------------------------------------------------------------|
 * |  RESERVED    | 3 Bytes |    33    | Reserved field.                                                                  |
 * |______________|_________|__________|__________________________________________________________________________________| 
 */
typedef struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed)) RSDPDescriptor_t;

typedef struct RSDPDescriptor20 {
    RSDPDescriptor_t FirstPart;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__ ((packed)) RSDPDescriptor20_t;

typedef struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__ ((packed)) ACPISDTHeader_t;

typedef struct RSDT {
    ACPISDTHeader_t h;
    uint32_t PointerToOtherSDT[4];                      // Pointer to other tables
} __attribute__ ((packed)) RSDT_t;

// typedef struct XSDT {
//   struct ACPISDTHeader h;
//   uint64_t* PointerToOtherSDT;
// } __attribute__ ((packed)) XSDT_t;

// PAGINA 173 ACPI 6.0

// PAGINA 205 ACPI 6.0
/**
 * @brief MadtLapic
 * Type: 0 Processor Local APIC structure
 * Length: 8
 * CpuUid: The OS associates this Local APIC Structure with a processor object in the 
 *         namespace when the _UID child object of the processor's device object (or 
 *         the ProcessorId listed in the Processor declaration operator) evaluates to a 
 *         numeric value that matches the numeric value in this field. Note that the 
 *         use of the Processor declaration operator is deprecated. See the 
 *         compatibility note in Section 5.2.12.2 and see Section 19.6.108, “Processor 
 *         (Declare Processor).”
 * ApicId: The processor’s local APIC ID.
 * Flags: 
 *      BIT 0 (Enabled): 
 *             supports enabling this processor during OS runtime.
 *             If this bit is clear and the Online Capable bit is also clear, this processor is 
 *             unusable, and OSPM shall ignore the contents of the Processor Local 
 *             APIC Structure
 * 
 *      BIT 1 (Online Capbable): 
 *             The information conveyed by this bit depends on the value of the 
 *             Enabled bit.
 *             If the Enabled bit is set, this bit is reserved and must be zero.
 *             Otherwise, if this this bit is set, system hardware supports enabling this 
 *             processor during OS runtime
 * 
 *      OTHER BITS : Reserved must be 0
 */
typedef struct MADT_LAPIC {
    uint8_t Type;
    uint8_t Length;
    uint8_t CpuUid;
    uint8_t ApicId;
    uint32_t Flags;
} __attribute__ ((packed)) MADT_LAPIC_t;

/**
 * @brief 
 * 
 */
typedef struct MADT {
    ACPISDTHeader_t h;
    uint32_t LocalInterruptControllerAddr;              // The 32-bit physical address at which each processor can access its local interrupt controller.
    uint32_t Flags;                                 
    MADT_LAPIC_t madtLapic;
} __attribute__ ((packed)) MADT_t;

typedef struct FADT {
    ACPISDTHeader_t h;
    uint32_t FIRMWARE_CTRL;
    uint32_t Dsdt;
    uint8_t Reserved;
    uint8_t Preferred_PM_Profile;
    uint16_t SCI_INT;
    uint32_t SMI_CMD;
    uint8_t ACPI_ENABLE;
    uint8_t ACPI_DISABLE;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_CNT;
    uint32_t PM1a_EVT_BLK;
    uint32_t PM1b_EVT_BLK;
    uint32_t PM1a_CNT_BLK;
    uint32_t PM1b_CNT_BLK;
    uint32_t PM2_CNT_BLK;
    uint32_t PM_TMR_BLK;
    uint32_t GPE0_BLK;
    uint32_t GPE1_BLK;
    uint8_t PM1_EVT_LEN;
    uint8_t PM1_CNT_LEN;
    uint8_t PM2_CNT_LEN;
    uint8_t PM_TMR_LEN;
    uint8_t GPE0_BLK_LEN;
    uint8_t GPE1_BLK_LEN;
    uint8_t GPE1_BASE;
    uint8_t CST_CNT;
    uint16_t P_LVL2_LAT;
    uint16_t P_LVL3_LAT;
    uint16_t FLUSH_SIZE;
    uint16_t FLUSH_STRIDE;
    uint8_t DUTY_OFFSET;
    uint8_t DUTY_WIDTH;
    uint8_t DAY_ALRM;
    uint8_t MON_ALRM;
    uint8_t CENTURY;
    uint16_t IAPC_BOOT_ARCH;
    uint8_t Reserved2;
    uint32_t Flags;
    char RESET_REG[12];
    uint8_t RESET_VALUE;
    uint16_t ARM_BOOT_ARCH;
    uint8_t FADTMinorVersion;
} __attribute__ ((packed)) FADT_t;

typedef struct DSDT {
    ACPISDTHeader_t h;
} __attribute__ ((packed)) DSDT_t;

void acpi_locate_rsdp(int* revision, RSDPDescriptor_t* rsdp, RSDPDescriptor20_t* rsdp20, char** errorMsg);

void acpi_locate_rsdt();

void acpi_dump();

#endif
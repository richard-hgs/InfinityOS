#include <stdint.h>
#include "memutils.h"
#include "hpet.h"

/*
 * Needs the acpi
 */

/*
 * The High Precision Event Timer driver.
 * This driver is closely modelled after the rtc.c driver.
 * http://www.intel.com/hardwaredesign/hpetspec_1.pdf
 */
#define	HPET_USER_FREQ	(64)
#define	HPET_DRIFT	(500)

#define HPET_RANGE_SIZE		1024	/* from HPET spec */

#define	HPET_OPEN		    0x0001
#define	HPET_IE			    0x0002	/* interrupt enabled */
#define	HPET_PERIODIC		0x0004
#define	HPET_SHARED_IRQ		0x0008

typedef struct address_structure
{
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed)) address_structure_t;
 
typedef struct description_table_header
{
    char signature[4];    // 'HPET' in case of HPET table
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    uint64_t oem_tableid;
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) description_table_header_t;
 
typedef struct hpet {
    description_table_header_t header;
    uint8_t hardware_rev_id;
    uint8_t comparator_count:5;
    uint8_t counter_size:1;
    uint8_t reserved:1;
    uint8_t legacy_replacement:1;
    uint16_t pci_vendor_id;
    address_structure_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__((packed)) hpet_t;

void getInfo() {
    
}
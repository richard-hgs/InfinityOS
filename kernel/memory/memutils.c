#include <stdint.h>
#include <stddef.h>
#include "ports.h"
#include "gdt.h"
#include "io.h"
#include "memutils.h"

extern unsigned char __BASE_ADDR[];
extern unsigned char __MAX_ADDR[];

void* memcpy(void* dst, const void* src, uint32_t size) {
    uint32_t* wdst = dst;
    const uint32_t* wsrc = src;

    // word per word copy if both addresses aligned
    if (!((uint32_t) wdst & 3) && !((uint32_t) wsrc & 3))
    {
        while (size > 3)
        {
            *wdst++ = *wsrc++;
            size -= 4;
        }
    }

    unsigned char *cdst = (unsigned char*)wdst;
    unsigned char *csrc = (unsigned char*)wsrc;

    // byte per byte for last bytes (or not aligned)
    while (size--) {
        *cdst++ = *csrc++;
    }
    return dst;
}

void* memcpy_def(void* dst, const void* src, unsigned char defVal, uint32_t size) {
    uint32_t* wdst = dst;
    const uint32_t* wsrc = src;

    // word per word copy if both addresses aligned
    if (!((uint32_t) wdst & 3) && !((uint32_t) wsrc & 3))
    {
        while (size > 3)
        {
            *wdst++ = *wsrc++;
            size -= 4;
        }
    }

    unsigned char *cdst = (unsigned char*)wdst;
    unsigned char *csrc = (unsigned char*)wsrc;

    // byte per byte for last bytes (or not aligned)
    while (size--) {
        unsigned char sourceVal = *csrc++;
        if (sourceVal == 0) {
            sourceVal = defVal;
        }
        *cdst++ = sourceVal;
    }
    return dst;
}

void* memcpy_r(void* dst, const void* src, uint32_t size) {
    uint32_t* wdst = dst;
    const uint32_t* wsrc = src;

    // word per word copy if both addresses aligned
    if (!((uint32_t) wdst & 3) && !((uint32_t) wsrc & 3))
    {
        while (size > 3)
        {
            *wdst-- = *wsrc--;
            size -= 4;
        }
    }

    unsigned char *cdst = (unsigned char*)wdst;
    unsigned char *csrc = (unsigned char*)wsrc;

    // byte per byte for last bytes (or not aligned)
    while (size--) {
        *cdst-- = *csrc--;
    }
    return dst;
}

void* memcpy_16(void* dst, const void* src, uint32_t size) {
    uint32_t* wdst = dst;
    const uint32_t* wsrc = src;

    // word per word copy if both addresses aligned
    if (!((uint32_t) wdst & 3) && !((uint32_t) wsrc & 3))
    {
        while (size > 3)
        {
            *wdst++ = *wsrc++;
            size -= 4;
        }
    }

    uint16_t *cdst = (uint16_t*)wdst;
    uint16_t *csrc = (uint16_t*)wsrc;

    // byte per byte for last bytes (or not aligned)
    while (size--) {
        *cdst++ = *csrc++;
    }
    return dst;
}

void* memcpy_16_def(void* dst, const void* src, uint16_t defVal, uint32_t size) {
    uint32_t* wdst = dst;
    const uint32_t* wsrc = src;

    // word per word copy if both addresses aligned
    if (!((uint32_t) wdst & 3) && !((uint32_t) wsrc & 3))
    {
        while (size > 3)
        {

            *wdst++ = *wsrc++;
            size -= 4;
        }
    }

    uint16_t *cdst = (uint16_t*) wdst;
    uint16_t *csrc = (uint16_t*) wsrc;

    // byte per byte for last bytes (or not aligned)
    while (size--) {
        uint16_t sourceVal = *csrc++;
        if (sourceVal == 0) {
            sourceVal = defVal;
        }
        *cdst++ = sourceVal;
    }
    return dst;
}


void* memset(void* dst, uint32_t val, uint32_t size) {
    // build 8 bits and 32 bits values
    uint8_t byte = (uint8_t)(val & 0xFF);
    uint32_t word = (val<<24) | (val<<16) | (val<<8) | val;

    // word per word if address aligned
    uint32_t* wdst = (uint32_t *) dst;

    if((((uint32_t) dst) & 0x3) == 0) {
        while(size > 3) {
            *wdst++ = word;
            size -= 4;
        }
    }

    // byte per byte for last bytes (or not aligned)
    char* cdst = (char *)wdst;

    while(size--) {
        *cdst++ = byte;
    }

    return dst;
}

void* memsetw(void *dest, uint16_t val, uint16_t len) {
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

int memcmp(const void* s1, const void* s2, uint32_t n) {
    const uint8_t * cs1 = s1;
    const uint8_t * cs2 = s2;

    while (n > 0) {
        if (*cs1++ != *cs2++)
            return (*--cs1 - *--cs2);
        n--;
    }
    return 0;
}

void memprintAsHex(uint32_t offset, uint32_t len, int vgaWidth) {
    unsigned char memValAt[1] = {0};
    uint32_t currentOffset = (uint32_t) offset;
    uint32_t currentLineOffset = offset;
    int currentColumn = 0;
    while(currentOffset < offset + len) {
        if (currentColumn >= vgaWidth - 8) {
            kprintf("\n");
            currentColumn = 0;
            currentLineOffset = currentOffset;
        }

        if (currentColumn == 0) {
            currentColumn += kprintf("%x -> ", currentLineOffset);
        }
        
        memcpy(memValAt, (void*) currentOffset, 1);
        currentColumn += kprintf("%x ", (int32_t) memValAt[0]);

        currentOffset++;
    }
    kprintf("\n");
}

void memprintAsStr(uint32_t offset, uint32_t len, int vgaWidth) {
    // char memValAt[1] = {0};
    uint32_t currentOffset = (uint32_t) offset;
    uint32_t currentLineOffset = offset;
    int currentColumn = 0;
    while(currentOffset < offset + len) {
        if (currentColumn >= vgaWidth - 8) {
            kprintf("\n");
            currentColumn = 0;
            currentLineOffset = currentOffset;
        }

        if (currentColumn == 0) {
            currentColumn += kprintf("%x -> ", currentLineOffset);
        }
        
        // memcpy(memValAt, (void*) currentOffset, 1);
        currentColumn += kprintf("%c", *((char*) currentOffset));

        currentOffset++;
    }
}
void memprintAsStr2(uint32_t offset, uint32_t len, int vgaWidth) {
    char memValAt[1] = {0};
    uint32_t currentOffset = (uint32_t) offset;
    uint32_t currentLineOffset = offset;
    int currentColumn = 0;
    while(currentOffset < offset + len) {
        if (currentColumn >= vgaWidth - 16) {
            // kprintf("\n");
            currentColumn = 0;
            currentLineOffset = currentOffset;
        }

        // if (currentColumn == 0) {
        //     currentColumn += kprintf("%x -> ", currentLineOffset);
        // }
        
        memcpy(memValAt, (void*) currentOffset, 1);
        currentColumn += kprintf("%s ", (char) memValAt[0]);

        currentOffset++;
    }
}

void hexDump( const char * desc, const void * addr, const int len, int perLine) {
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

            if (i != 0) kprintf("  %s\n", buff);

            // Output the offset of current line.

            kprintf("  %04X ", i & 0xFFFF);
        }

        // Now the hex code for the specific character.

        kprintf(" %02X", pc[i] & 0xFF);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        kprintf("   ");
        i++;
    }

    // And print the final ASCII buffer.

    kprintf("  %s\n", buff);
}

uint32_t probeRamSize() {
    uint32_t offset = ((uint32_t) __MAX_ADDR) + 1;
    uint32_t currentAddress = offset;
    uint32_t step = 1 * 1000 * 1000; // 1 mb step
    uint32_t i=0;
    for(i=0; i<900000000; i++) {
        int* addressPtr = (void*) currentAddress;
        *addressPtr = 1;
        int* addessProbePtr = (void*) currentAddress;
        if (*addessProbePtr == 1) {
            *addessProbePtr = 0;
        } else {
            break;
        }

        currentAddress += step;
    }

    return i;
}

// static inline unsigned long _readl(unsigned long addr) {
// 	return *(volatile unsigned long *)addr;
// }
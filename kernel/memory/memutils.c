#include <stdint.h>
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
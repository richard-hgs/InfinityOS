#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "bitwise.h"
#include "io.h"
#include "array.h"
#include "memutils.h"
#include "checksum.h"

void checksum_8bit(unsigned char* buffer, size_t bufferSize, size_t* bytesToIgnore, size_t bytesToIgnoreSize) {
    uint32_t sum = 0;
    for (size_t i=0; i<bufferSize; i++) {
        bool shouldIgnore = false;
        for (size_t x=0; x<bytesToIgnoreSize; x++) {
            if (bytesToIgnore[x] == i) {
                shouldIgnore = true;
                break;
            }
        }

        if (!shouldIgnore) {
            sum += buffer[i];
        }
    }

    uint8_t lowByte = (sum & 0xFF);
    uint8_t mCheckSum = 0xFF - lowByte;

    kprintf("sum: 0x%x - lowest2Bytes: %x - checksum: %x\n", sum, lowByte, mCheckSum);
}

uint32_t checksum_8bit_mod256(unsigned char* buffer, size_t bufferSize, size_t* bytesToIgnore, size_t bytesToIgnoreSize) {
    uint32_t sum = 0;
    for (size_t i=0; i<bufferSize; i++) {
        bool shouldIgnore = false;
        for (size_t x=0; x<bytesToIgnoreSize; x++) {
            if (bytesToIgnore[x] == i) {
                shouldIgnore = true;
                break;
            }
        }

        if (!shouldIgnore) {
            sum += buffer[i] % 256;
            // kprintf(" (0x%x | 0x%x)", buffer[i], sum);
        }
    }

    // kprintf("sum: 0x%x\n", sum);

    // kprintf("sum: 0x%x\n", sum % 256);

    return sum % 256;
}
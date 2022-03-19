#include <stdbool.h>
#include "io.h"
#include "bitwise.h"

void uint8_t_setbit(uint8_t* value, uint8_t bitPos, bool bitVal) {
    if (bitVal) {
        *value |= ((uint8_t)1) << bitPos;
    } else {
        *value &= ~(((uint8_t)1) << bitPos);
    }
}

bool uint8_t_getbit(uint8_t value, uint8_t bitPos) {
    return (value >> bitPos) & 1;
}

void uint8_t_tobin(uint8_t in, bool* out) {
    for(int n = 0; n < 8; n++) {
        out[n] = (in >> n) & 1;
    }
}

// Sum two binary numbers
void binary_sum(bool* binaryResult, bool* bin1, size_t size1, bool* bin2, size_t size2, size_t* written) {
    bool carry = 0;
    size_t i = 0;
    while(i<8 || carry > 0) {
        int bit1 = 0;
        int bit2 = 0; 
        
        if (i < size1) {
            bit1 = bin1[i]; 
        }
        
        if (i < size2) {
            bit2 = bin2[i];
        }

        if (bit1 == 0 && bit2 == 0 && carry == 0) {
            binaryResult[i] = 0;
            carry = 0;
        } else if ((bit1 == 1 && bit2 == 0 && carry == 0) || (bit2 == 1 && bit1 == 0 && carry == 0)) {
            binaryResult[i] = 1;
            carry = 0;
        } else if (bit1 == 1 && bit2 == 1 && carry == 0) {
            binaryResult[i] = 0;
            carry = 1;
        } else if (bit1 == 0 && bit2 == 0 && carry > 0) {
            binaryResult[i] = 1;
            carry = 0;
        } else if (((bit1 == 1 && bit2 == 0) || (bit2 == 1 && bit1 == 0)) && carry > 0) {
            binaryResult[i] = 0;
            carry = 1;
        } else if (bit1 == 1 && bit2 == 1 && carry > 0) {
            binaryResult[i] = 1;
            carry = 1;
        }
        
        i++;
        (*written)++;
    }
}

void binary_sum_uint8_t_array(bool* sumBuffer, size_t buffSize, uint8_t* arr, size_t size, size_t* written, bool sumCarry) {
    for (size_t i=0; i<size; i++) {
        *written = 0;

        bool binVal1[8] = {0};
        uint8_t_tobin(arr[i], binVal1);
        
        // char buffer[9] = {0};
        // binToString(buffer, 9, binVal1, 8);
        
        // kprintf("%s\n", buffer);
        
        binary_sum(sumBuffer, sumBuffer, buffSize, binVal1, 8, written);
    }
}

void bin_to_string(char* buffer, size_t size, bool* binVal, size_t binSize) {
    if (binSize > size) {
        return;
    }

    size_t i = 0;
    for(i=0; i<binSize; i++) {
        buffer[i] = (char) (binVal[binSize - (i + 1)] == 1 ? 0x31 : 0x30);
    }

    if ((binSize - size) > 0) {
        buffer[i] = '\0';
    }
}

/*
Usage example: 
// uint8_t arr[35] = {0x52, 0x53, 0x44, 0x34, 0x00, 0x00, 0x00, 0x01, 0x48, 0x42, 0x4f, 0x43, 0x48, 0x53, 0x20, 0x42, 0x58, 0x50, 0x43, 0x20, 0x20, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, 0x42, 0x58, 0x50, 0x43, 0x01, 0x00, 0x00, 0x00};
// uint8_t arr[52] = {0x52, 0x53, 0x44, 0x34, 0x00, 0x00, 0x00, 0x01, 0x48, 0x42, 0x4f, 0x43, 0x48, 0x53, 0x20, 0x42, 0x58, 0x50, 0x43, 0x20, 0x20, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, 0x42, 0x58, 0x50, 0x43, 0x01, 0x00, 0x00, 0x00, 0x00, 0x17, 0x3A, 0x01, 0x00, 0x64, 0xff, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x64, 0xff, 0x08, 0x00};

    unsigned char arr[4] = {0x99, 0xE2, 0x24, 0x84};

    int binVal1[8];
    uint8_t_tobin(0xE2, binVal1);
    
    char buffer[9];
    binToString(buffer, 9, binVal1, 8);
    
    printf("%s\n", buffer);
    
    int binVal2[8];
    uint8_t_tobin(0x24, binVal2);
    
    char buffer2[9];
    binToString(buffer2, 9, binVal2, 8);
    
    printf("%s\n", buffer2);
    
    int binaryBufferSum[30];
    char buffer3[31];
    size_t written = 0;
    
    binarySum(binaryBufferSum, binVal1, 8, binVal2, 8, &written);
    binToString(buffer3, 31, binaryBufferSum, written);
    
    printf("--------\n%s\n", buffer3);
    
    
*/

void int64_t__to__int32_t(int64_t value, int32_t* highBits, int32_t* lowerBits) {
    *highBits = (int32_t) ((value >> 32) & 0xFFFFFFFF);
    *lowerBits = (int32_t) ((value) & 0xFFFFFFFF);
}

void int32_t__to__int64_t(int32_t highBits, int32_t lowerBits, int64_t* value) {
    *value = ((highBits & 0xFFFFFFFFFFFF) << 32) | ((lowerBits & 0xFFFFFFFFFFFF));
}
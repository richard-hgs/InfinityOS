#ifndef _BITWISE_H_
#define _BITWISE_H_
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define low_16(addr) (uint16_t)((addr) & 0xFFFF)
#define high_16(addr) (uint16_t)(((addr) >> 16) & 0xFFFF)

void uint8_t_setbit(uint8_t* value, uint8_t bitPos, bool bitVal);

bool uint8_t_getbit(uint8_t value, uint8_t bitPos);

void uint8_t_tobin(uint8_t in, bool* out);

void binary_sum(bool* binaryResult, bool* bin1, size_t size1, bool* bin2, size_t size2, size_t* written);

void bin_to_string(char* buffer, size_t size, bool* binVal, size_t binSize);

void binary_sum_uint8_t_array(bool* sumBuffer, size_t buffSize, uint8_t* arr, size_t size, size_t* written, bool sumCarry);

void int64_t__to__int32_t(int64_t value, int32_t* highBits, int32_t* lowerBits);

void int32_t__to__int64_t(int32_t highBits, int32_t lowerBits, int64_t* value);



#endif
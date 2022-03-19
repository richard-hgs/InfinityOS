#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_
#pragma once

void checksum_8bit(unsigned char* buffer, size_t bufferSize, size_t* bytesToIgnore, size_t bytesToIgnoreSize);

uint32_t checksum_8bit_mod256(unsigned char* buffer, size_t bufferSize, size_t* bytesToIgnore, size_t bytesToIgnoreSize);

#endif
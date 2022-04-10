#ifndef _MEMUTILS_H_
#define _MEMUTILS_H_

#pragma once

#include <stdint.h>

// Executable file example: 
//
//               +-----------------------------+
// High Address: |   Command-line arguments    |
//               |  and environment variables  |
//               +-----------------------------+
//               |            Stack            |
//               |              |              |
//               |              V              |
//               |                             |
//               |              ^              |
//               |              |              |
//               |            Heap             |
//               +-----------------------------+
//               |     Uninitialized Data      |
//               +-----------------------------+
//               |      Initialized Data       |
//               +-----------------------------+
//               |        Program Text         |
//  Low Address: |       (machine code)        |
//               +-----------------------------+

/*******************************************************************************************
 * This function copies a source buffer to a destination buffer.
 * There is no alignment constraint, but the performance is improved if the buffers
 * are both aligned on a 32 bits word boundary.
 *******************************************************************************************
 * @ dst     : pointer on destination buffer.
 * @ src     : pointer on source buffer.
 * @ size    : number of bytes.
 * @ return pointer on destination buffer.
 ******************************************************************************************/
void* memcpy(void* dst, const void* src, uint32_t size);

void* memcpy_def(void* dst, const void* src, unsigned char defVal, uint32_t size);

void* memcpy_r(void* dst, const void* src, uint32_t size);

void* memcpy_16(void* dst, const void* src, uint32_t size);

void* memcpy_16_def(void* dst, const void* src, uint16_t defVal, uint32_t size);


/*******************************************************************************************
 * This function sets a constant value in each byte of a target buffer.
 *******************************************************************************************
 * @ dst     : pointer on destination buffer.
 * @ val     : constant value (cast to uint8_t).
 * @ size    : number of bytes.
 * @ return pointer on destination buffer.
 ******************************************************************************************/
void* memset(void* dst, uint32_t val, uint32_t size);

/*******************************************************************************************
 * TODO
 ******************************************************************************************/
int memcmp(const void* s1, const void* s2, uint32_t n);

void* memsetw(void *dest, uint16_t val, uint16_t len);

void memprintAsHex(uint32_t offset, uint32_t len, int vgaWidth);

void memprintAsStr(uint32_t offset, uint32_t len, int vgaWidth);

void memprintAsStr2(uint32_t offset, uint32_t len, int vgaWidth);

void hexDump(const char * desc, const void * addr, const int len, int perLine);

uint32_t probeRamSize();

// static inline unsigned long _readl(unsigned long addr);

#endif
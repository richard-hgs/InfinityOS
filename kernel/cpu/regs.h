/*
 * regs.h - Simple implementation for register structure.
 *
 * Author: Infinity Technology
 * Date: 12/20/2020
 *
 ************************************************************************
 */

#ifndef _REGS_H_
#define _REGS_H_
#pragma once

// #include "types.h"

#include <stdint.h>

/* 32 bit registers. */
typedef struct tagREG32 {
	uint32_t eax, ebx, ecx, edx;
	uint32_t esi, edi, ebp, esp;
	uint32_t cs, ss, ds, es, cflag, eflag;
} regs_t;

#endif

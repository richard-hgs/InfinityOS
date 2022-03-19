/*
 * idt.h - Header file for IDT implementation (data types and defines).
 *
 * Author: Infinity Technology
 * Date  : 07/03/2020
 *
 ***********************************************************************
 */

#ifndef _IDT_H_
#define _IDT_H_
#pragma once

// #include "types.h"
#include <stdint.h>

/* Segment selectors. */
#define KERNEL_CS 0x08

/* Interrupt gate handler definition. */
typedef struct idt_gate {
	uint16_t low_offset;
	uint16_t sel;
	uint8_t always0;
	uint8_t flags;
	uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

/* A point to the array of interrupt handlers. */
typedef struct idt_reg {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idt_reg_t;

#define IDT_ENTRIES 256

/* Functions implemented in idt.c. */
extern void set_idt_gate(int n, uint32_t handler);
extern void set_idt(void);

#endif

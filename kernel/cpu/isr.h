/*
 * isr.h - Header file for ISR implementation.
 *
 * Author: Infinity Technology
 * Date  : 07/03/2020
 *
 **********************************************************************
 */

#ifndef _ISR_H_
#define _ISR_H_
#pragma once

// #include "types.h"
#include <stdint.h>

/* Macros to append number to end of ISRs and IRQs. */
#define TO_ISR(X) isr ##X
#define TO_IRQ(X) irq ##X

/* Macros to set the ISRs and IRQs. */
#define SET_ISR(X) set_idt_gate((X), (uint32_t)TO_ISR(X))
#define SET_IRQ(X) set_idt_gate((X)+32, (uint32_t)TO_IRQ(X))

/* Macro to set a custom gate. */
#define SET_GATE(X,N) set_idt_gate(X, (uint32_t)N);

/* ISRs reserved for CPU exceptions. */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* IRQ definitions */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

					// INTERRUPTIONS FOR DATA REQUEST RECEIVES
					// ---- PIC MASTER ----
#define IRQ0 32		// Temporizador de intervalos 8253/8254 (temporizador do sistema)
#define IRQ1 33		// Teclado
#define IRQ2 34		// Reservada para a 8259B (amarrada ao IRQ 9)
#define IRQ3 35		// COM2 e COM4
#define IRQ4 36		// COM1 e COM3
#define IRQ5 37		// LPT2 ou placa de som
#define IRQ6 38		// Disquetes
#define IRQ7 39		// LPT1
					// ---- PIC secundário ----
#define IRQ8 40		// Relógio de tempo real (real time clock, RTC)
#define IRQ9 41		// Amarrada ao IRQ2
#define IRQ10 42	// Network interface
#define IRQ11 43    // USB port, sound card
#define IRQ12 44	// Mouse PS/2
#define IRQ13 45 	// Co-processador matemático
#define IRQ14 46	// Drives IDE primários
#define IRQ15 47	// Drives IDE secundários

/* Struct which aggregates many registers. */
typedef struct registers {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

/* Functions implemented in isr.c */
extern void isr_install(void);
extern void isr_handler(registers_t *r);
extern void irq_install(void);

typedef void (*isr_t)(registers_t *);
extern void register_interrupt_handler(uint8_t n, isr_t handler);

#endif

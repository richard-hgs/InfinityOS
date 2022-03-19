/*
 * timer.h - Header file for the PIT implementation.
 *
 * Author: Infinity Technology
 * Date  : 07/04/2020
 *
 *********************************************************************
 */

#ifndef _TIMER_H_
#define _TIMER_H_
#pragma once

#include <stdbool.h>
#include <stdint.h>
// #include "types.h"
#include "regs.h"

/* Get timer ticks from kernel. */
uint64_t get_timer_ticks(void);
/* Get timer seconds from kernel. */
uint64_t get_timer_seconds(void);
/* Get system registers current values. */
regs_t *get_current_regs(bool update);
/* Call this function to install a timer. */
void install_timer(uint32_t freq);

#endif

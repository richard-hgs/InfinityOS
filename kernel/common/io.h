/*
 * io.h - Header file for standard output functions.
 *
 * Author: Infinity Technology
 * Date  : 07/01/2020
 *
 ******************************************************************
 */

#ifndef _IO_H_
#define _IO_H_
#pragma once

/* Print character to console */
int kputc(char c);
/* Print string to console */
int kputs(const char *s);
/* Print formatted string to console */
int kprintf(const char *formatted, ...);
/* Print formatted string to debug serial port console */
int dskprintf(const char* formatted, ...);


#endif

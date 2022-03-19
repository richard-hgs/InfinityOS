/*
 * helper.h - Header file for miscellaneous functions.
 *
 * Author: Infinity Technology
 * Date  : 07/04/2020
 *
 *********************************************************************
 */

#ifndef _HELPER_H_
#define _HELPER_H_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Standard NULL */

#ifdef NULL
#undef NULL
#endif

#ifdef __cplusplus
#define NULL 0
#else
#define NULL (void*)0
#endif

/* String functions */
extern int strlen(const char *s);
extern int strstrip(char *s);
extern int strcmp(const char *s, const char *t);
extern void reverse(char *s);
extern int itoa_s(int n, unsigned char radix, char *str, int size);
extern int atoi(char *str);
extern void uitoa_s(unsigned int value, unsigned char radix, char *str, int size);
extern int itoa_s2(int value, unsigned char radix, char *str, int size, char leadChar, int leadCount);

void u64toa(uint64_t value, char* str, unsigned char radix);

/* Functions for kernel. */
extern void delay(uint64_t ticks);
extern void sleep(uint64_t secs);
extern void append(char *s, char c);
extern void backspace(char *s);

#ifdef __cplusplus
}
#endif

#endif

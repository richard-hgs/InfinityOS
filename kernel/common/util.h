/*
 * util.h - Common utility functions.
 *
 * Author: Infinity Technology
 * Date  : 07/03/2020
 *
 **********************************************************
 */

#ifndef _UTIL_H_
#define _UTIL_H_
#pragma once

/* Functions implemented in util.c */
// void memcpy(void *dest, const void *src, unsigned long nbytes);
// void memset(void *dest, int c, int size);

/// Return the number of elements in the C array.
#define arrsize(x)           ((int)(sizeof(x) / sizeof(x[0])))

/// Compile-time static assertion
#define STATIC_ASSERT(a, b)  _Static_assert(a, b)

/// Forced structure packing (use only when absolutely necessary)
#define PACKSTRUCT           __attribute__((packed, aligned(1)))

#endif

#ifndef _K_MALLOC_H_
#define _K_MALLOC_H_
#pragma once

#include <stddef.h>

void* kmalloc(size_t size);

void kfree(void* memPtr);

#endif
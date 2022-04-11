#include <stdint.h>
#include <stdbool.h>
// #include <stddef.h>
#include "bitwise.h"
#include "memutils.h"
#include "kheap.h"
#include "io.h"
#include "kmalloc.h"

void* kmalloc(size_t size) {
    return kheap_addBlock(size, true, true);
}

void kfree(void* memPtr) {
    kheap_freeBlock(memPtr);
}
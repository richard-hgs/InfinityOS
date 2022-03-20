#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bitwise.h"
#include "memutils.h"
#include "kheap.h"
#include "io.h"
#include "kmalloc.h"

void* kmalloc(size_t size) {
    return kheap_addBlock(size);
}

void kmalloc_test() {
    char* myAllocMemPointer = kheap_addBlock(16);
    memcpy(myAllocMemPointer, "a1b2c3d4e5f6g7h8", 16);
    char* myAllocMemPointer2 = kheap_addBlock(16);
    memcpy(myAllocMemPointer2, "i1b2c3d4e5f6g7h8", 16);

    kprintf("%s\n", myAllocMemPointer);
    kprintf("%s\n", myAllocMemPointer2);

    kprintf("HEAP - blockCount: %i - alloc: %i - used: %i - malloc: %i - mused: %i\n", kheapMem.blockCount, kheapMem.heapSpaceAllocated, kheapMem.heapSpaceUsed, kheapMem.heapMemAllocated, kheapMem.heapMemUsed);

    kprintf("HEAP - lastBLockInUse: %i\n", kheap_getBlockFlagBit(kheapMem.lastBlock, 0));

    kheap_freeBlock(myAllocMemPointer2);

    kprintf("HEAP - lastBLockInUse: %i\n", kheap_getBlockFlagBit(kheapMem.lastBlock, 0));

    kprintf("HEAP - blockCount: %i - alloc: %i - used: %i - malloc: %i - mused: %i\n", kheapMem.blockCount, kheapMem.heapSpaceAllocated, kheapMem.heapSpaceUsed, kheapMem.heapMemAllocated, kheapMem.heapMemUsed);

    // kprintf("HEAP - kernelEndAddress: %x - lastBLockAddress: %x - lastBlockDataAddress: %x - nextBLock: %x\n", __MAX_ADDR, heapMem.lastBlock, myAllocMemPointer, heapMem.lastBlock->nextBlock);

    // k_heapBMAddBlock(&kheap, 0x100000, 0x100000, 256);

    // // KHEAPBLOCKBM* fb = kheap.fblock;

    // // kprintf("maxAddr: %i - %x - %x\n", __MAX_ADDR, __MAX_ADDR, 0x100000);
    // // kprintf("firstBlock -> size: %i - used: %i - blockSize: %i - lfb: %i - sizeHeapBlockBm: %i\n", fb->size, fb->used, fb->bsize, fb->lfb, sizeof(KHEAPBLOCKBM));

    // char* myCharPtr = (char*) k_heapBMAlloc(&kheap, sizeof(char) * 16);
    // memcpy(myCharPtr, "meuTexto", 8);

    // kprintf("myCharPtrAddr: \n" + ((uint32_t) (void*) myCharPtr2));

    // // memprintAsHex((void*) (0x100000), 600, 85);
}
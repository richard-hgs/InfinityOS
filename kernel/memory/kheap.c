#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bitwise.h"
#include "memutils.h"
#include "io.h"
#include "kheap.h"

extern unsigned char __BASE_ADDR[];
extern unsigned char __MAX_ADDR[];

bool heapInitialized = false;
size_t blockHeaderSize = sizeof(HeapMemBlock_t);
size_t heapStartAddress = ((size_t) __MAX_ADDR) + 1;

// void kheap_updateCheckSum(HeapMemBlock_t* block) {

// }

void kheap_setBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos, bool bitVal) {
    uint8_t_setbit(&block->flags, bitPos, bitVal);
}

bool kheap_getBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos) {
    return uint8_t_getbit(block->flags, bitPos);
}

/**
 * Add a new block to the current heap memory
 * @param blockSize         The size of the data to be allocated for this block
 * @return blockAddress     The address of the start memory location of this block
 */
void* kheap_addBlock(size_t blockSize) {
    HeapMemBlock_t* heapMemBlock = 0;

    if (!heapInitialized) {
        heapMemBlock = (HeapMemBlock_t*) heapStartAddress;
    } else {
        heapMemBlock = kheapMem.lastBlock->nextBlock;
    }

    heapMemBlock->nextBlock = (HeapMemBlock_t*)  (((int) (void*) heapMemBlock) + blockHeaderSize + blockSize + 1);
    // Set first bit to 1=BLOCK_IN_USE
    kheap_setBlockFlagBit(heapMemBlock, 0, 1);

    if (!heapInitialized) {
        kheapMem.firstBlock = heapMemBlock;
        kheapMem.blockCount = 1;
        kheapMem.heapSpaceAllocated = blockSize;
        kheapMem.heapSpaceUsed = blockSize;
        kheapMem.heapMemAllocated = blockHeaderSize + blockSize;
        kheapMem.heapMemUsed = blockHeaderSize + blockSize;
        heapInitialized = true;
    } else {
        kheapMem.blockCount += 1;
        kheapMem.heapSpaceAllocated += blockSize;
        kheapMem.heapSpaceUsed += blockSize;
        kheapMem.heapMemAllocated += blockHeaderSize + blockSize;
        kheapMem.heapMemUsed += blockHeaderSize + blockSize;
    }
    
    kheapMem.lastBlock = heapMemBlock;

    return (void*) (((size_t) heapMemBlock) + blockHeaderSize);
}

void kheap_freeBlock(void* ptr) {
    size_t ptrAddress = ((size_t) ptr);
    HeapMemBlock_t* heapBlock = (HeapMemBlock_t*) (ptrAddress - blockHeaderSize);
    // heapBlock->flags = 0;
    kheap_setBlockFlagBit(heapBlock, 0, 0);
    size_t heapSpaceAllocated = (((size_t) heapBlock->nextBlock)) - ptrAddress - 1;
    kheapMem.heapSpaceUsed -= heapSpaceAllocated;
    kheapMem.heapMemUsed -= heapSpaceAllocated;
}

size_t kheap_get_kernel_static_mem_usage() {
    return ((size_t) __MAX_ADDR) - ((size_t) __BASE_ADDR);
}
#ifndef _KHEAP_H_
#define _KHEAP_H_
#pragma once

typedef struct HeapMemBlock {
    struct HeapMemBlock* nextBlock;     // Pointer to the next block
    uint8_t flags;                      // Block Flag (BIT_1 -> 0=BLOCK_FREE, 1=BLOCK_IN_USE)
} __attribute__ ((packed)) HeapMemBlock_t;

typedef struct HeapMem {
    HeapMemBlock_t* firstBlock;
    HeapMemBlock_t* lastBlock;
    size_t blockCount;
    size_t heapSpaceAllocated;
    size_t heapSpaceUsed;
    size_t heapMemAllocated;
    size_t heapMemUsed;
} HeapMem_t;

HeapMem_t kheapMem;

void kheap_setBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos, bool bitVal);

bool kheap_getBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos);

void* kheap_addBlock(size_t blockSize, bool reuseFreeBlocks);

void kheap_freeBlock(void* ptr);

void* kheap_getMemBlockDataAddress(HeapMemBlock_t* heapMemBlock);

size_t kheap_getMemBlockDataSize(HeapMemBlock_t* heapMemBlock);

size_t kheap_get_kernel_static_mem_usage();

#endif
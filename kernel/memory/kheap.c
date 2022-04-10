#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bitwise.h"
#include "memutils.h"
#include "io.h"
#include "kheap.h"

#define FLAG_IN_USE 0

extern unsigned char __BASE_ADDR[];
extern unsigned char __MAX_ADDR[];

bool heapInitialized = false;
size_t blockHeaderSize = sizeof(HeapMemBlock_t);
size_t heapStartAddress = ((size_t) __MAX_ADDR) + 1;

// void kheap_updateCheckSum(HeapMemBlock_t* block) {

// }

/**
 * @brief Change block flags bit pos to true|false
 * 
 * @param block     Block
 * @param bitPos    Flags bit pos
 * @param bitVal    1 or 0
 */
void kheap_setBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos, bool bitVal) {
    uint8_t_setbit(&block->flags, bitPos, bitVal);
}

/**
 * @brief Get block flags bit pos
 * 
 * @param block Block
 * @param bitPos Flags bit pos
 * @return true 
 * @return false 
 */
bool kheap_getBlockFlagBit(HeapMemBlock_t* block, uint8_t bitPos) {
    return uint8_t_getbit(block->flags, bitPos);
}

/**
 * @brief Search and return free block in the memory that can contain this allocated size or NULL if not found.
 * 
 * @return HeapMemBlock_t* 
 */
HeapMemBlock_t* kheap_searchNextFreeBlockSize(size_t size) {
    if (kheapMem.blockCount == 0) {
        return NULL;
    }
    HeapMemBlock_t* currentBlock = kheapMem.firstBlock;
    size_t currentBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentBlock);
    size_t lastBlockAddress = (size_t) kheap_getMemBlockDataAddress(kheapMem.lastBlock);
    while(currentBlockAddress <= lastBlockAddress) {
        if (kheap_getMemBlockDataSize(currentBlock) >= size && kheap_getBlockFlagBit(currentBlock, FLAG_IN_USE) == false) {
            return currentBlock;
        }
        currentBlock = currentBlock->nextBlock;
        currentBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentBlock);

    }
    return NULL;
}

/**
 * Add a new block to the current heap memory
 * @param blockSize         The size of the data to be allocated for this block
 * @param reuseFreeBlocks   true=Reuse free blocks if available, false=Create a new block
 * @return blockAddress     The address of the start memory location of this block
 */
void* kheap_addBlock(size_t blockSize, bool reuseFreeBlocks) {
    bool isReuseBlock = false;
    HeapMemBlock_t* heapMemBlock = 0;

    if (!heapInitialized) {
        heapMemBlock = (HeapMemBlock_t*) heapStartAddress;
    } else {
        heapMemBlock = kheapMem.lastBlock->nextBlock;
    }

    // If reuse free blocks
    if (heapInitialized && reuseFreeBlocks) {
        // Search for a free block that can holds this data block size
        HeapMemBlock_t* heapMemBlockReuse = kheap_searchNextFreeBlockSize(blockSize);
        if (heapMemBlockReuse != NULL) {
            heapMemBlock = heapMemBlockReuse;
            isReuseBlock = true;
        }
    }

    if (!isReuseBlock) {
        // Create a reference to a next block to save the current block size and next block reference
        heapMemBlock->nextBlock = (HeapMemBlock_t*)  (((int) (void*) heapMemBlock) + blockHeaderSize + blockSize + 1);
    }

    // Set first bit to 1=BLOCK_IN_USE
    kheap_setBlockFlagBit(heapMemBlock, FLAG_IN_USE, true);

    if (!heapInitialized) {
        kheapMem.firstBlock = heapMemBlock;
        kheapMem.blockCount = 1;
        kheapMem.heapSpaceAllocated = blockSize;
        kheapMem.heapSpaceUsed = blockSize;
        kheapMem.heapMemAllocated = blockHeaderSize + blockSize;
        kheapMem.heapMemUsed = blockHeaderSize + blockSize;
        heapInitialized = true;
    } else if (!isReuseBlock) {
        // Increment this block size in memory totals
        kheapMem.blockCount += 1;
        kheapMem.heapSpaceAllocated += blockSize;
        kheapMem.heapSpaceUsed += blockSize;
        kheapMem.heapMemAllocated += blockHeaderSize + blockSize;
        kheapMem.heapMemUsed += blockHeaderSize + blockSize;
    }
    
    if (!isReuseBlock) {
        // If is a new block set this block as the last memory block
        kheapMem.lastBlock = heapMemBlock;
    }

    // Return the address to this block data offset
    return kheap_getMemBlockDataAddress(heapMemBlock);
}

/**
 * @brief Frees an allocated memory block by its data offset
 * 
 * @param ptr Pointer offset of this HeapMemBlock_t data
 */
void kheap_freeBlock(void* ptr) {
    size_t ptrAddress = ((size_t) ptr);
    HeapMemBlock_t* heapBlock = (HeapMemBlock_t*) (ptrAddress - blockHeaderSize);
    // Change the block FLAG_IN_USE to false
    kheap_setBlockFlagBit(heapBlock, FLAG_IN_USE, false);
    // Decrement this memory from the total of memory usage
    size_t heapSpaceAllocated = (((size_t) heapBlock->nextBlock)) - ptrAddress - 1;
    kheapMem.heapSpaceUsed -= heapSpaceAllocated;
    kheapMem.heapMemUsed -= heapSpaceAllocated;
}

/**
 * @brief Get the memory offset of the data of this HeapMemBlock_t
 * 
 * @param heapMemBlock  The heap memory block
 * @return void*        The pointer to the data of HeapMemBlock_t block
 */
void* kheap_getMemBlockDataAddress(HeapMemBlock_t* heapMemBlock) {
    return (void*) (((size_t) heapMemBlock) + blockHeaderSize);
}

/**
 * @brief Gets the size of the data of the data of this heap memory block
 * 
 * @param heapMemBlock  The block with a header(blockHeaderSize bytes) + data(n bytes)
 * @return size_t       Block data size
 */
size_t kheap_getMemBlockDataSize(HeapMemBlock_t* heapMemBlock) {
    size_t blockDataAddr = (size_t) heapMemBlock;
    size_t nextBlockDataAddr = (size_t) heapMemBlock->nextBlock;
    return (nextBlockDataAddr - blockDataAddr) - (blockHeaderSize + 1);
}

/**
 * @brief Gets the size of memory allocated statically for the kernel by the bootloader
 * 
 * @return size_t Kernel size in RAM memory
 */
size_t kheap_get_kernel_static_mem_usage() {
    return ((size_t) __MAX_ADDR) - ((size_t) __BASE_ADDR);
}


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

void createNextBlock(HeapMemBlock_t* blockToBindNextBlock, size_t dataSize) {
    blockToBindNextBlock->nextBlock = (HeapMemBlock_t*) (((int) (void*) blockToBindNextBlock) + blockHeaderSize + dataSize + 1);
}

/**
 * @brief Search and return free block in the memory that can contain the size or NULL if not found.
 * 
 * @param mergeFreeBlocks If a free block is found and its size is below required size and the preceding blocks
 *                        are also free and the sum of all free blocks is >= required size than merge these blocks
 * @param allocatedSizeIncrement The allocation size increased of this new free block if merged;
 * @return HeapMemBlock_t* 
 */
HeapMemBlock_t* kheap_searchNextFreeBlockSize(size_t size, bool mergeFreeBlocks, size_t* allocatedSizeIncrement) {
    if (kheapMem.blockCount == 0) {
        return NULL;
    }
    HeapMemBlock_t* currentBlock = kheapMem.firstBlock;
    size_t currentBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentBlock);
    size_t lastBlockAddress = (size_t) kheap_getMemBlockDataAddress(kheapMem.lastBlock);
    size_t currentBlockDataSize = 0;
    bool currentBlockInUse = true;
    while(currentBlockAddress <= lastBlockAddress) {
        currentBlockDataSize = kheap_getMemBlockDataSize(currentBlock);
        currentBlockInUse = kheap_getBlockFlagBit(currentBlock, FLAG_IN_USE);

        if (currentBlockInUse == false) {
            // Block is free to be used
            if (currentBlockDataSize >= size) {
                // Block has size to contain the requested allocated data size
                return currentBlock;
            } else if (mergeFreeBlocks) {
                // Block don't has size to contain the requested allocated data size 
                // Try to merge next free blocks preceding this block to make room for the data
                // This method reduces compute time while prevent the iteration in entire memory to find a block that can holds the allocated data size.
                HeapMemBlock_t* currentNextBlock = currentBlock;
                size_t currentNextBlockAddress = currentBlockAddress;
                size_t currentNextBlockDataSize = currentBlockDataSize;
                size_t sizeOfAllFreeBlocks = currentBlockDataSize;
                size_t countOfAllFreeMergeBlocks = 0;
                bool currentNextBlockInUse = currentBlockInUse;
                while(currentNextBlockAddress <= lastBlockAddress && sizeOfAllFreeBlocks < size && !currentNextBlockInUse) {
                    currentNextBlock = currentNextBlock->nextBlock;
                    currentNextBlockInUse = kheap_getBlockFlagBit(currentNextBlock, FLAG_IN_USE);
                    currentNextBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentNextBlock);
                    kprintf("currentNextBlokAddress: %i\n", currentNextBlockAddress);
                    currentNextBlockDataSize = kheap_getMemBlockDataSize(currentNextBlock);
                    if (!currentNextBlockInUse) {
                        countOfAllFreeMergeBlocks++;
                        sizeOfAllFreeBlocks += currentNextBlockDataSize;
                    }
                }

                if (sizeOfAllFreeBlocks >= size) {
                    // Merged blocks can contain the requested allocated data size
                    // Update current block to the block that precedes the last free merge block;
                    currentBlock->nextBlock = currentNextBlock->nextBlock;
                    // Update the memory totals removing the headers merged headers from the allocated totals
                    kheapMem.blockCount -= countOfAllFreeMergeBlocks;
                    // Return the sum of all headers blocks as a allocation
                    *allocatedSizeIncrement = countOfAllFreeMergeBlocks * blockHeaderSize;
                    // If the address of the last merge block is equal to last block address than set this new
                    // merged block as the last block of the memory.
                    if (currentNextBlockAddress == lastBlockAddress) {
                        createNextBlock(currentBlock, sizeOfAllFreeBlocks + (*allocatedSizeIncrement));
                        kheapMem.lastBlock = currentBlock;
                    }
                    kprintf("sizeOfLastMergeBlock: %i\n", kheap_getMemBlockDataSize(currentNextBlock));
                    return currentBlock;
                }
            }
        }
        currentBlock = currentBlock->nextBlock;
        currentBlockAddress = (size_t) kheap_getMemBlockDataAddress(currentBlock);

    }
    return NULL;
}


/**
 * Add a new block to the current heap memory.
 * @param size         The size of the data to be allocated for this block.
 * @param reuseFreeBlocks   true=Reuse free blocks if available, false=Create a new block.
 * @param mergeFreeBlocks   true=If a free block is found and its size is below required size and the preceding blocks
 *                          are also free and the sum of all free blocks is >= required size than merge these blocks.
 *                          false=Disable merge functionality.
 *                          OBS: Merge functionaly only works if reuseFreeBlocks=true.
 * @return blockAddress     The address of the start memory location of this block.
 */
void* kheap_addBlock(size_t size, bool reuseFreeBlocks, bool mergeFreeBlocks) {
    bool isReuseBlock = false;
    HeapMemBlock_t* heapMemBlock = 0;
    size_t allocatedSizeToIncrement = 0;

    if (!heapInitialized) {
        heapMemBlock = (HeapMemBlock_t*) heapStartAddress;
    } else {
        heapMemBlock = kheapMem.lastBlock->nextBlock;
    }

    // If reuse free blocks
    if (heapInitialized && reuseFreeBlocks) {
        // Search for a free block that can holds this data block size
        HeapMemBlock_t* heapMemBlockReuse = kheap_searchNextFreeBlockSize(size, true, &allocatedSizeToIncrement);
        if (heapMemBlockReuse != NULL) {
            heapMemBlock = heapMemBlockReuse;
            isReuseBlock = true;
        }
    }

    if (!isReuseBlock) {
        // Create a reference to a next block to save the current block size and next block reference
        createNextBlock(heapMemBlock, size);
    }

    size_t heapMemBlockSize = kheap_getMemBlockDataSize(heapMemBlock);

    // Set first bit to 1=BLOCK_IN_USE
    kheap_setBlockFlagBit(heapMemBlock, FLAG_IN_USE, true);

    if (!heapInitialized) {
        kheapMem.firstBlock = heapMemBlock;
        kheapMem.blockCount = 1;
        kheapMem.heapSpaceAllocated = heapMemBlockSize;
        kheapMem.heapSpaceUsed = heapMemBlockSize;
        kheapMem.heapMemAllocated = blockHeaderSize + heapMemBlockSize;
        kheapMem.heapMemUsed = blockHeaderSize + heapMemBlockSize;
        heapInitialized = true;
    } else {
        if (!isReuseBlock) {
            // Increment this block size in memory totals
            kheapMem.blockCount += 1;
            kheapMem.heapSpaceAllocated += heapMemBlockSize;
            kheapMem.heapSpaceUsed += heapMemBlockSize;
            kheapMem.heapMemAllocated += blockHeaderSize + heapMemBlockSize;
            kheapMem.heapMemUsed += blockHeaderSize + heapMemBlockSize;
        } else {
            // Is reused block the header is already in the heapMemUsed increment only the allocated size
            kheapMem.heapSpaceAllocated += allocatedSizeToIncrement;
            kheapMem.heapSpaceUsed += heapMemBlockSize;
            // kheapMem.heapMemAllocated -= allocatedSizeToIncrement;
            kheapMem.heapMemUsed += heapMemBlockSize - allocatedSizeToIncrement;
            kprintf("reuseBlockSize: %i\n", heapMemBlockSize);
        }
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
    // size_t heapSpaceAllocated = (((size_t) heapBlock->nextBlock)) - ptrAddress - 1;
    size_t heapSpaceAllocated = kheap_getMemBlockDataSize(heapBlock);
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


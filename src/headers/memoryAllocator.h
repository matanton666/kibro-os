#pragma once
#include "dList.h"
#include "std.h"
#include "screen.h"

#define FREE_BLOCK_HEADER_SIZE sizeof(FreeBlockHeader)
#define USED_BLOCK_HEADER_SIZE sizeof(UsedBlockHeader)
#define FOOTER_SIZE sizeof(Footer)

#define MIN_ALLOC_SIZE FREE_BLOCK_HEADER_SIZE + FOOTER_SIZE + 4 // min alloc size of 4B

struct FreeBlockHeader
{
	size_t size : 31; // taking the LSB for a used flag
	bool inUse : 1;
	DList node;
} __attribute__((packed));

struct UsedBlockHeader
{
	size_t size : 31; // taking the LSB for a used flag
	bool inUse : 1;
} __attribute__((packed));

struct Footer  // same footer for used/free blocks
{
	size_t size : 31; // taking the LSB for a used flag
	bool inUse : 1;
} __attribute__((packed));


class Allocator
{
private:
	
	size_t _freeMem, _usedMem, _reservedMem;
	DList* _freeBlockList; // needs to be initialized as the node of the first chunk
	uintptr_t _memStartAddr, _memEndAddr;

	Footer* getFooter(UsedBlockHeader* block);
	Footer* getFooter(FreeBlockHeader* block);
	UsedBlockHeader* getUsedHeader(Footer* footer);
	FreeBlockHeader* getFreeHeader(Footer* footer);

	Footer* createFooter(UsedBlockHeader* block);
	Footer* createFooter(FreeBlockHeader* block);

	// splits a free block to used and free block, used block's size will be `size`
	// beware this also updates the _freeMem and _usedMem values
	UsedBlockHeader* splitBlock(FreeBlockHeader* block, size_t size);
	bool coalesceAdjacent(UsedBlockHeader* block);
	void* coalescePredecessor(UsedBlockHeader* block);
	bool coalesceSuccessor(UsedBlockHeader* block, bool PredecessorMerged);

	void* mallocAddr(void* addr, size_t size, FreeBlockHeader* block);

public:
    // initializes the allocator with a memory pool
	void init(uintptr_t start, size_t length);

	// allocates a block of memory of size `size`
	void* malloc(size_t size);

	// allocates a block of memory of size `size` aligned to `alignment` (ex. 0x1000)
	void* mallocAligned(size_t size, size_t alignment);

	void* calloc(size_t size);

	// allocates a block of memory of size `size` aligned to `alignment` (ex. 0x1000)
	void* callocAligned(size_t size, size_t alignment);

	// frees a block of memory
	void free(void* ptr);

	// adds new memory to the heap,
	// **`start` must be the memory address right after `_memEndAddr` (start == _memEndAddr + 1)*
	void appendNewMem(uintptr_t start, size_t size);

	size_t getFreeMem();
	size_t getUsedMem();
	size_t getReservedMem();
	size_t getTotalMem();

};


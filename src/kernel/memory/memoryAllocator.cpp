#include "memoryAllocator.h"


Footer* Allocator::getFooter(UsedBlockHeader* block)
{
	return (Footer*)((uintptr_t)block + USED_BLOCK_HEADER_SIZE + block->size);
}

Footer* Allocator::getFooter(FreeBlockHeader* block)
{
	return (Footer*)((uintptr_t)block + FREE_BLOCK_HEADER_SIZE + block->size);
}

UsedBlockHeader* Allocator::getUsedHeader(Footer* footer)
{
	return (UsedBlockHeader*)((uintptr_t)footer - footer->size - USED_BLOCK_HEADER_SIZE);
}

FreeBlockHeader* Allocator::getFreeHeader(Footer* footer)
{
	return (FreeBlockHeader*)((uintptr_t)footer - footer->size - FREE_BLOCK_HEADER_SIZE);
}

Footer* Allocator::createFooter(UsedBlockHeader* block)
{
	Footer* footer = getFooter(block);
	footer->size = block->size;
	footer->inUse = true;
	return footer;
}

Footer* Allocator::createFooter(FreeBlockHeader* block)
{
	Footer* footer = getFooter(block);
	footer->size = block->size;
	footer->inUse = false;
	return footer;
}



void Allocator::init(uintptr_t start, size_t length)
{
	uintptr_t alignedStart = align_up(start, sizeof(uintptr_t)); // aligning the addr of the mem pool
	length = length - (alignedStart - start); // adjusting the length of the mem pool
	size_t blockSize = length - FREE_BLOCK_HEADER_SIZE - FOOTER_SIZE; // size of block not including header and footer
	
	// create a block that populates the entire free memory pool

	FreeBlockHeader* block = (FreeBlockHeader*)alignedStart; // putting the block's header at the start of the 
	block->size = blockSize;
	block->inUse = false;
	DListNodeInit(&block->node);

	createFooter(block); // putting the block's footer at the end of the mem block
	
	_freeBlockList = &(block->node);

	_freeMem = blockSize;
	_usedMem = 0;
	_reservedMem = length - blockSize;

	_memStartAddr = alignedStart; // setting the pointers to the first and last addr of the memory pool;
	_memEndAddr = alignedStart + length;
}

void Allocator::appendNewMem(uintptr_t start, size_t size)
{
	if (start != _memEndAddr + 1) // if the new memory block is not adjacent to the current memory pool
	{
		screen.print("\n Warning: adding non-adjacent memory block to the allocator\n");
		return;
	}

	// if old mem pool has free mem at end then merge with new mem pool
	Footer* lastBlock = (Footer*)((uintptr_t)_memEndAddr - FOOTER_SIZE);
	if (lastBlock->inUse == false)
	{
		FreeBlockHeader* lastFreeBlock = getFreeHeader(lastBlock);
		lastFreeBlock->size += size;
		createFooter(lastFreeBlock);
		_freeMem += size;
		_memEndAddr = start + size;
		return;
	}

	// else create new free block
	FreeBlockHeader* block = (FreeBlockHeader*)start;
	block->size = size - FREE_BLOCK_HEADER_SIZE - FOOTER_SIZE; // size of block not including header and footer
	block->inUse = false;
	createFooter(block);

	DListNodeInit(&(block->node));
	DListPushBack(&(_freeBlockList), &(block->node)); // adding to free memory the list

	_freeMem += block->size;
	_reservedMem += FREE_BLOCK_HEADER_SIZE + FOOTER_SIZE;
	_memEndAddr = start + size;
}

UsedBlockHeader* Allocator::splitBlock(FreeBlockHeader* block, size_t size)
{
	size_t totalSize = block->size;

	if (size >= totalSize - USED_BLOCK_HEADER_SIZE - FOOTER_SIZE - MIN_ALLOC_SIZE * 2) // if the size is too big
		return nullptr;
	
	DListRemove(&(_freeBlockList), &(block->node)); // remove the block because the address is now used block

	UsedBlockHeader* newBlock = (UsedBlockHeader*)block; // "convert" the first part to a used block
	newBlock->size = size;
	newBlock->inUse = true;
	Footer* newFooter = createFooter(newBlock);

	// create second half (free block part)
	FreeBlockHeader* freeBlock = (FreeBlockHeader*)((uintptr_t)newFooter + FOOTER_SIZE);
	freeBlock->size = totalSize - (size + USED_BLOCK_HEADER_SIZE + FOOTER_SIZE);
	freeBlock->inUse = false;
	createFooter(freeBlock);

	DListNodeInit(&(freeBlock->node));
	DListPushFront(&(_freeBlockList), &(freeBlock->node));

	_freeMem -= newBlock->size + USED_BLOCK_HEADER_SIZE + FOOTER_SIZE;
	_usedMem += newBlock->size;
	_reservedMem += USED_BLOCK_HEADER_SIZE + FOOTER_SIZE;

	return newBlock;
}


bool Allocator::coalesceAdjacent(UsedBlockHeader* block)
{
	UsedBlockHeader* newBlock = (UsedBlockHeader*)coalescePredecessor(block);
	return coalesceSuccessor((newBlock == nullptr ? block : newBlock), (newBlock != nullptr)) || (newBlock != nullptr);
}

void* Allocator::coalescePredecessor(UsedBlockHeader* block)
{
	if ((uintptr_t)block == _memStartAddr) // if this is the first memory block there isn't a predecessor, so no merge.
	{
		return nullptr;
	}

	Footer* currFooter = getFooter(block);
	Footer* predecessorFooter = (Footer*)((uintptr_t)block - FOOTER_SIZE);

	if(predecessorFooter->inUse == false)
	{
		// merge entire used block, new free block size = current free block + used block size + used header + footer
		FreeBlockHeader* predecessorHeader = getFreeHeader(predecessorFooter);

		predecessorHeader->size += (block->size + USED_BLOCK_HEADER_SIZE + FOOTER_SIZE);

		_freeMem += currFooter->size + FOOTER_SIZE + USED_BLOCK_HEADER_SIZE;
		_usedMem -= currFooter->size;// used mem includes the size of the headers and footers
		_reservedMem -= FOOTER_SIZE + USED_BLOCK_HEADER_SIZE;

		// curr footer is now the new free blocks footer
		currFooter->size = predecessorHeader->size;
		currFooter->inUse = false;

		return (void*)predecessorHeader;
	}

	return nullptr;
}

bool Allocator::coalesceSuccessor(UsedBlockHeader* block, bool predecessorMerged)
{
	// if predecessor was merged then block is now a freeBlock that we need to merge
	unsigned int currHeaderType = (predecessorMerged ? FREE_BLOCK_HEADER_SIZE : USED_BLOCK_HEADER_SIZE);

	// if the curr block is the last block in the memory pool it doesn't have a successor, so no merge
	if (((uintptr_t)block + block->size + currHeaderType + FOOTER_SIZE) == _memEndAddr) {
		return false;
	}
	
	FreeBlockHeader* successorHeader = (FreeBlockHeader*)((uintptr_t)block + currHeaderType + block->size + FOOTER_SIZE);
	
	if (successorHeader->inUse == false)
	{
		Footer* successorFooter = getFooter(successorHeader);
		size_t blockSize = block->size;

		FreeBlockHeader* newBlock = (FreeBlockHeader*)block; // convert current block to free block if it wasnt already
		newBlock->inUse = false;
		//                size of curr block                            + size of successor block    
		newBlock->size = ((blockSize + currHeaderType + FOOTER_SIZE) + successorHeader->size); 
		successorFooter->size = newBlock->size;

		DListRemove(&(_freeBlockList), &(successorHeader->node)); // remove successor from free list because it dosent exist after merge

		// if curr block was already free type then dont need to init the node again
		if(!predecessorMerged)
		{
			DListNodeInit(&(newBlock->node)); 
			DListPushFront(&(_freeBlockList), &(newBlock->node));

			// if not merged before, than block's type is used, so add the entire block size to free mem
			_freeMem += blockSize + FOOTER_SIZE + USED_BLOCK_HEADER_SIZE;
			_usedMem -= blockSize;
			_reservedMem -= FOOTER_SIZE + USED_BLOCK_HEADER_SIZE;
		}
		else 
		{
			// if merged before, than type is free, so add only the headers that are gained from the merge
			_freeMem += FOOTER_SIZE + FREE_BLOCK_HEADER_SIZE;
			_reservedMem -= FOOTER_SIZE + FREE_BLOCK_HEADER_SIZE;
		}
		
		return true;
	}
	return false;
}

void* Allocator::mallocAddr(void* addr, size_t size, FreeBlockHeader* block)
{
	if (addr)
	{
		size_t blockSize = block->size;
		// if can, split the block.     (size after converting into a used block is big enough)
		if (((blockSize + (FREE_BLOCK_HEADER_SIZE - USED_BLOCK_HEADER_SIZE)) - size) >= MIN_ALLOC_SIZE)
		{
			UsedBlockHeader* newBlock = splitBlock(block, size);
			if (newBlock == nullptr) 
				return nullptr;

			addr = (void*)((uintptr_t)newBlock + USED_BLOCK_HEADER_SIZE); // setting the data ptr after block type has changed from free to used
			return addr;
		}
		return nullptr;
	}
	return addr;
}



void* Allocator::malloc(size_t size)
{
	void* dataPtr = nullptr;
	FreeBlockHeader* block = nullptr;

	if (size == 0 || size > _freeMem) // invalid size
		return nullptr;

	//search for a block that can fit the request
	DList* currNode = _freeBlockList;
	while (currNode != nullptr)
	{
		block = (FreeBlockHeader*)((uintptr_t)currNode - offsetof(FreeBlockHeader, node)); // get current block

		if ((block->size + (FREE_BLOCK_HEADER_SIZE - USED_BLOCK_HEADER_SIZE)) >= size) // if size after converting into a used block is big enough
		{
			dataPtr = (void*)((uintptr_t)block + USED_BLOCK_HEADER_SIZE); // get the pointer to the data of the block, the block now is a USED block
			break;
		}
		currNode = currNode->next;
	}

	return mallocAddr(dataPtr, size, block);
}

void* Allocator::mallocAligned(size_t size, size_t alignment)
{
	void* dataPtr = nullptr;

	FreeBlockHeader* block = nullptr;

	if (size == 0 || size > _freeMem) // invalid size
		return nullptr;

	//search for a block that can fit the request
	DList* currNode = _freeBlockList;
	while (currNode != nullptr)
	{
		block = (FreeBlockHeader*)((uintptr_t)currNode - offsetof(FreeBlockHeader, node)); // get current block

		void* alignedAddr = (void*)align_up((uintptr_t)block + USED_BLOCK_HEADER_SIZE, alignment);
		if ((uintptr_t)alignedAddr >= _memEndAddr) // if the aligned addr is out of the memory pool
			return nullptr;

		size_t alignedDataSize = block->size - ((uintptr_t)alignedAddr - (uintptr_t)block);
		
		// because it splits the block to 3 blocks need to check that before and after are big enough
		if (alignedDataSize >= size && alignedDataSize < block->size && (((uintptr_t)alignedAddr - USED_BLOCK_HEADER_SIZE) - (uintptr_t)block) > MIN_ALLOC_SIZE) // if size after converting into a used block is big enough
		{
			dataPtr = alignedAddr; // get the pointer to the data of the block, the block now is a USED block
			break;
		}

		currNode = currNode->next;
	}

	// need to split the free block into 2 free blocks, the first is just excess memory from the alignment and the second is the memory in which we want to allocate a new used block 
	if (dataPtr) {


		// go back "used size" because that is where we want the data to start when it turns into a "used block"
		FreeBlockHeader* alignedBlock = (FreeBlockHeader*)((uintptr_t)dataPtr - USED_BLOCK_HEADER_SIZE);
		alignedBlock->size = block->size - (((uintptr_t)alignedBlock + FREE_BLOCK_HEADER_SIZE) - (uintptr_t)block);
		createFooter(alignedBlock);
		DListNodeInit(&(alignedBlock->node));
		DListPushFront(&(_freeBlockList), &(alignedBlock->node));

		// first block (the one before the to be allocated block)
		Footer* firstPartFooter = (Footer*)((uintptr_t)alignedBlock - FOOTER_SIZE);
		block->size = (uintptr_t)firstPartFooter - ((uintptr_t)block + FREE_BLOCK_HEADER_SIZE);
		createFooter(block);

		_reservedMem += FREE_BLOCK_HEADER_SIZE + FOOTER_SIZE;
		_freeMem -= FREE_BLOCK_HEADER_SIZE + FOOTER_SIZE;

		void* a = mallocAddr(dataPtr, size, alignedBlock);
		return a;
	}
	return nullptr;
}


void* Allocator::calloc(size_t size)
{
	void* ptr = malloc(size);
	if (ptr)
	{
		memset(ptr, 0, size);
	}
	return ptr;
}


void* Allocator::callocAligned(size_t size, size_t alignment)
{
	void* ptr = mallocAligned(size, alignment);
	if (ptr)
	{
		memset(ptr, 0, size);
	}
	return ptr;
}

void Allocator::free(void* ptr)
{
	if (ptr)
	{
		UsedBlockHeader* usedBlock = (UsedBlockHeader*)((uintptr_t)ptr - USED_BLOCK_HEADER_SIZE);
		unsigned int usedBlockSize = usedBlock->size;
		if (usedBlock->inUse && !coalesceAdjacent(usedBlock))
		{
			// convert block to free block
			FreeBlockHeader* freeBlock = (FreeBlockHeader*)usedBlock; // "convert" the used block to a free block
			freeBlock->size = usedBlock->size - (FREE_BLOCK_HEADER_SIZE - USED_BLOCK_HEADER_SIZE); // size after alignment and header, footer
			freeBlock->inUse = false;
			createFooter(freeBlock);

			DListNodeInit(&(freeBlock->node));
			DListPushFront(&(_freeBlockList), &(freeBlock->node));

			_freeMem += freeBlock->size;
			_usedMem -= usedBlockSize;
			_reservedMem += (FREE_BLOCK_HEADER_SIZE - USED_BLOCK_HEADER_SIZE);
		}
	}
}

size_t Allocator::getFreeMem()
{
	return _freeMem;
}

size_t Allocator::getUsedMem()
{
	return _usedMem;
}


size_t Allocator::getReservedMem()
{
	return _reservedMem;
}


size_t Allocator::getTotalMem()
{
	return _freeMem + _usedMem + _reservedMem;
}


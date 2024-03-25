#include "../../headers/fsHelper.h"




Inode getInode(unsigned int globalInodeIndex) // starts at 1
{
	unsigned int blockGroupNum = getBlockGroupIndex(globalInodeIndex);
	unsigned int inodeIdx = (globalInodeIndex) % global_sb.inodesInBg;
	return getInode(inodeIdx, getBGD(blockGroupNum));
}

uint32_t getBlockGroupIndex(uint32_t globalInodeIndex)
{
	return (globalInodeIndex - 1) / global_sb.inodesInBg;
}

size_t readBlockList(uint32_t* blockLists, uint8_t** buffer, size_t size)
{
	uint32_t s = size;

	for (int i = 0; i < NUM_DIRECT_POINTERS ; i++) {
		uint32_t blockAddr = blockLists[i];
		if (blockAddr == 0)
			break;

		uint32_t amountToRead = (s >= BLOCK_SIZE) ? BLOCK_SIZE : s;

		// Read data from the block on disk into the buffer
		disk.read(blockAddr, amountToRead, *buffer);

		// Update buffer pointer and remaining size
		*buffer += (amountToRead / sizeof(uint8_t));
		s -= amountToRead;

		// Check if we have read enough bytes
		if (s <= 0) 
			return size;
	}
	return size - s;
}


uint8_t* getInodeData(Inode inode)
{
	size_t dataSize = getInodeSize(inode);
	size_t currSize = dataSize;
	uint8_t* Data = (uint8_t*)kernelPaging.getAllocator()->calloc(dataSize);
	uint8_t* dataPtrCopy = Data;

	currSize -= readBlockList(inode.directBlockPtrs, &Data, currSize);
	// TODO: this part should be done only after we finished successful file and dir creation
	// if (currSize != 0 && inode.SIBP != 0)
	// 	currSize -= readSIBP(inode.SIBP, &Data, currSize);

	// if (currSize != 0 && inode.DIBP != 0)
	// 	currSize -= readDIBP(inode.DIBP, &Data, currSize);

	// if (currSize != 0 && inode.TIBP != 0)
	// 	currSize -= readTIBP(inode.TIBP, &Data, currSize);

	return dataPtrCopy;
}

Directory* readDirectoryList(Inode inode)
{
	//check if inode is indeed a directory
	if ((((inode.typeAndPermissions >> 12) & 0xF) << 12) != DIR)
		return nullptr;

	Directory* head = nullptr;
	Directory* current = nullptr;
	size_t offset = 0;

	uint8_t* buffer = getInodeData(inode);
	size_t bufferSize = getInodeSize(inode);

	while (offset < bufferSize)
	{
		// Create a Directory node
		Directory* node = (Directory*)kernelPaging.getAllocator()->calloc(sizeof(Directory)); 
		memcpy(&node->entry, buffer + offset, sizeof(DirectoryEntry));
		node->next = nullptr;

		if (!head) // insert to linked list
		{
			head = node;
			current = node;
		}
		else
		{
			current->next = node;
			current = current->next;
		}

		// Move to the next directory entry (size of the direcoty = sizeof(directory) - sizeof(char*) + entry.nameLength)
		offset += sizeof(DirectoryEntry);
	}

	kernelPaging.getAllocator()->free(buffer); 
	return head;
}

void freeDirectoryList(Directory* head)
{
	Directory* temp = head;
	while (temp != nullptr)
	{
		Directory* next = temp->next;
		kernelPaging.getAllocator()->free(temp);
		temp = next;
	}
}

uint32_t getFreeBlockIdx(uint8_t* bitmap)
{
	for (int i = 1; i <= global_sb.blocksInBg; i++)
	{
		if(BitMapDS::getBit(bitmap, i) == 0)
			return i;
	}
	return 0;
}

uint32_t getFreeInodeIdx(uint8_t* bitmap)
{
	for (int i = 1; i <= global_sb.inodesInBg; i++)
	{
		if(BitMapDS::getBit(bitmap, i) == 0)
			return i;
	}
	return SIZE_ERROR;
}

 
bool writeInode(uint8_t* data, size_t size, uint32_t globalInodeIndex)
{
	Inode inode = getInode(globalInodeIndex);
	uint32_t groupIndex = (globalInodeIndex - 1) / global_sb.inodesInBg;
	BGD bgd = getBGD(groupIndex);

	// find free block
	uint8_t *blockBitmap = getBlockBitmap(bgd);

	inode.lowerFileSize = size;
	inode.upperFileSize = size >> 32;
	// write data to block
	int i = 0;
	while (size > 0 && i < NUM_DIRECT_POINTERS)
	{
		uint32_t amountToWrite = (size >= BLOCK_SIZE) ? BLOCK_SIZE : size;
		uint32_t blockGlobalAddr = 0;

		if (inode.directBlockPtrs[i] == 0) // inode does not have a block allocated
		{
			uint32_t blockIndex = getFreeBlockIdx(blockBitmap); // TODO: create a request function that automatically fetches and sets
			BitMapDS::setBit(blockBitmap, blockIndex, 1);
			if (blockIndex == 0)
			{
				write_serial("no free blocks");
				return false;
			}
			setBlockBitmap(bgd, blockBitmap);
			blockGlobalAddr = BLOCK_OFFSET(blockIndex +  bgd.inodeTable + (getInodeTableSize(global_sb) / BLOCK_SIZE));
		}
		else // overrite current block in use
		{
			blockGlobalAddr = inode.directBlockPtrs[i];
		}
		
		// add group data part offset to block index
		global_sb.totalFreeBlocks--;
		bgd.freeBlocks--;

		setSuperblock(global_sb);
		setBGD(groupIndex, bgd);

		if (!disk.write(blockGlobalAddr, amountToWrite, data))
		{
			write_serial("write to disk faild");
			return false;
		}
			

		inode.directBlockPtrs[i] = blockGlobalAddr;

		size -= amountToWrite;
		data += amountToWrite;
		i++;
	}

	// deallocate all blocks not in use by the inode
	// TODO: not sure this works, need to test but its not important for now
	// for (int j = i; j < NUM_DIRECT_POINTERS; j++)
	// {
	// 	if (inode.directBlockPtrs[j] != 0)
	// 	{
	// 		// get local offset of block and deallocate 
	// 		BitMapDS::setBit(blockBitmap, REV_BLOCK_OFFSET(inode.directBlockPtrs[j]) - bgd.inodeTable - (getInodeTableSize(global_sb) / BLOCK_SIZE), 0);
	// 		global_sb.totalFreeBlocks++;
	// 		bgd.freeBlocks++;
	// 	}
	// }

	setSuperblock(global_sb);
	setBGD(groupIndex, bgd);
	setBlockBitmap(bgd, blockBitmap);



	kernelPaging.getAllocator()->free(blockBitmap);
	setInode(globalInodeIndex, bgd, inode);
	return i != NUM_DIRECT_POINTERS;
}

size_t getInodeSize(Inode inode)
{
	return (inode.upperFileSize << 32) | inode.lowerFileSize;
}

void deleteInode(uint32_t globalInodeIndex)
{
	Inode inode = getInode(globalInodeIndex);
	uint32_t groupIndex = (globalInodeIndex - 1) / global_sb.inodesInBg;
	BGD bgd = getBGD(groupIndex);

	uint8_t *blockBitmap = getBlockBitmap(bgd);
	uint8_t *inodeBitmap = getInodeBitmap(bgd);

	// deallocate all blocks in use by the inode
	for (int i = 0; i < NUM_DIRECT_POINTERS; i++)
	{
		if (inode.directBlockPtrs[i] != 0)
		{
			// get local offset of block and deallocate 
			BitMapDS::setBit(blockBitmap, REV_BLOCK_OFFSET(inode.directBlockPtrs[i]) - bgd.inodeTable - (getInodeTableSize(global_sb) / BLOCK_SIZE), 0);
			global_sb.totalFreeBlocks++;
			bgd.freeBlocks++;
		}
	}

	// deallocate inode
	BitMapDS::setBit(inodeBitmap, globalInodeIndex, 0);
	bgd.freeInodes++;

	setSuperblock(global_sb);
	setBGD(groupIndex, bgd);
	setBlockBitmap(bgd, blockBitmap);
	setInodeBitmap(bgd, inodeBitmap);

	kernelPaging.getAllocator()->free(blockBitmap);
	kernelPaging.getAllocator()->free(inodeBitmap);
}
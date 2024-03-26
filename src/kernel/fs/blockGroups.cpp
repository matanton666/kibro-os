#include "../../headers/blockGroups.h"


Superblock global_sb;
BGDT global_bgdt;


uint32_t roundUp(uint32_t numToRound, uint32_t multiple)
{
	return numToRound = (numToRound + multiple - 1) & ~(multiple - 1);
}

Superblock createSuperblock()
{
	Superblock sb;
	// init new superblock with default values
	memset(&sb, 0, sizeof(Superblock));
	sb.ext2Sign = EXT_MAGIC;
	sb.sbContainerBlock = 1;
	sb.inodesInBg = 8192;
	sb.blocksInBg = 8192;
	sb.totalFreeBlocks = 8192;
	sb.totalFreeInodes = 8192;
	sb.sbContainerBlock = 1;
	sb.blockSize = 0; // block size of 1024 bytes
	sb.state = CLEAN;
	sb.errorHandle = PANIC;

	sb.totalBlocks = DISK_SIZE / (BLOCK_SIZE);// 51200 // TODO: calculate the actual size of the disk
	sb.totalInodes = (sb.inodesInBg * sb.totalBlocks) / sb.blocksInBg;
	

	write_serial("created superblock");

	return sb;
}

BGDT createBGDT()
{

	BGDT bgdt;
	unsigned int groupCount = (int)(1 + (global_sb.totalBlocks-1) / global_sb.blocksInBg);
	unsigned int descriptionListSize = groupCount * sizeof(BGD); // in bytes
	descriptionListSize = roundUp(descriptionListSize, BLOCK_SIZE); // round up to the nearest 1024 bytes
	bgdt.groupDescriptors = (BGD*)kernelPaging.getAllocator()->calloc(descriptionListSize); // buffer that will be written to the disk


	uint32_t groupSize = (global_sb.blocksInBg * BLOCK_SIZE); // 1024 * 8192 bytes
	// calculate inode table size (because it is a list with variable size)
	uint32_t inodeTableListSize = getInodeTableSize(global_sb);

	// 	                 superblockAddr + sizeofSuperblock + group descriptors size
	uint32_t group0_Offset = 1 + 1 + (descriptionListSize / BLOCK_SIZE);


	for (unsigned int groupNum = 0; groupNum < groupCount; groupNum++)
	{	
		// calculate the addresses of the header blocks
		uint32_t dataBlockBitmapOffset = group0_Offset + (groupNum * groupSize / BLOCK_SIZE);
		uint32_t inodeBitmapOffset = dataBlockBitmapOffset + 1;
		uint32_t inodeTableOffset = inodeBitmapOffset + 1;

		//                       blocksInGroup - InodeBitmap - DataBlockBitmap - InodeTable
		uint32_t blocksInData = global_sb.blocksInBg - 2 - getInodeTableSize(global_sb);


		// create block group descriptor for this group
		BGD bgd;
		bgd.blockUsageBitmap = dataBlockBitmapOffset;
		bgd.inodeUsageBitmap = inodeBitmapOffset;
		bgd.inodeTable = inodeTableOffset;
		bgd.freeBlocks = blocksInData; // amount of blocks in the data blocks
		bgd.freeInodes = global_sb.inodesInBg;
		bgd.totalDirs = 0;
		bgdt.groupDescriptors[groupNum] = bgd;
	}

	return bgdt;
}

Superblock getSuperblock()
{
	
	// read the superblock from the disk
	disk.read(SB_POSITION, sizeof(Superblock), (uint8_t*)&global_sb);
	return global_sb;
}

BlockGroup getBlockGroup(unsigned int blockGroupIdx)
{
	BlockGroup bg;

	BGD bgd = getBGD(blockGroupIdx);
	bg.dataBlockBitmap = (uint8_t*)BLOCK_OFFSET(bgd.blockUsageBitmap);
	bg.inodeBitmap = (uint8_t*)BLOCK_OFFSET(bgd.inodeUsageBitmap);
	bg.inodeTable = (Inode*)BLOCK_OFFSET(bgd.inodeTable);

	bg.bgd = (BGD*)((blockGroupIdx * sizeof(BGD)) + (SB_POSITION + BLOCK_SIZE));
	bg.data = (uint8_t*)((uint32_t)bg.inodeTable + getInodeTableSize(global_sb));

	return bg;
}

BGD getBGD(unsigned int blockGroupIdx)
{
	BGD bgd;
	uint32_t groupDescriptorsAddr = SB_POSITION + BLOCK_SIZE;

	// read the block group descriptor from the disk
	disk.read(groupDescriptorsAddr + (blockGroupIdx * sizeof(BGD)), sizeof(BGD), (uint8_t*)&bgd);
	return bgd;
}

uint8_t* getBlockBitmap(BGD bgd)
{
	uint8_t* blockBitmap = (uint8_t*)kernelPaging.getAllocator()->calloc(BLOCK_SIZE);
	disk.read(BLOCK_OFFSET(bgd.blockUsageBitmap), BLOCK_SIZE, blockBitmap);
	return blockBitmap;
}

uint8_t* getInodeBitmap(BGD bgd)
{
	uint8_t* inodeBitmap = (uint8_t*)kernelPaging.getAllocator()->calloc(BLOCK_SIZE);
	disk.read(BLOCK_OFFSET(bgd.inodeUsageBitmap), BLOCK_SIZE, inodeBitmap);
	return inodeBitmap;
}

InodeTable getInodeTable(BGD bgd)
{
	InodeTable it;
	it.inodes = (Inode*)kernelPaging.getAllocator()->calloc(getInodeTableSize(global_sb));
	disk.read(BLOCK_OFFSET(bgd.inodeTable), getInodeTableSize(global_sb), (uint8_t*)it.inodes);
	return it;
}

void setBGD(unsigned int blockGroupIdx, BGD bgd)
{
	// calculate the addresses of the header blocks
	uint32_t groupDescriptorsAddr = SB_POSITION + BLOCK_SIZE;

	// write the block group descriptor to the disk
	disk.write(groupDescriptorsAddr + (blockGroupIdx * sizeof(BGD)), sizeof(BGD), (uint8_t*)&bgd);
}

void setSuperblock(Superblock sb)
{
	// write the superblock to the disk
	disk.write(SB_POSITION, sizeof(Superblock), (uint8_t*)&sb);
	// disk.write(SB_POSITION, 300, (uint8_t*)&sb);
}

void setBGDT(BGDT bgdt)
{
	unsigned int groupCount = (int)(1 + (global_sb.totalBlocks-1) / global_sb.blocksInBg);
	unsigned int descriptionListSize = groupCount * sizeof(BGD); // in bytes
	// descriptionListSize = roundUp(descriptionListSize, BLOCK_SIZE); // round up to the nearest 1024 bytes
	// write the block group descriptor table to the disk
	disk.write(SB_POSITION + BLOCK_SIZE, descriptionListSize, (uint8_t*)bgdt.groupDescriptors);
	global_bgdt = bgdt;
}

void setBlockBitmap(BGD bgd, uint8_t* blockBitmap)
{
	disk.write(BLOCK_OFFSET(bgd.blockUsageBitmap), BLOCK_SIZE, blockBitmap);
}

void setInodeBitmap(BGD bgd, uint8_t* inodeBitmap)
{
	disk.write(BLOCK_OFFSET(bgd.inodeUsageBitmap), BLOCK_SIZE, inodeBitmap);
}

void setInodeTable(BGD bgd, InodeTable it)
{
	disk.write(BLOCK_OFFSET(bgd.inodeTable), getInodeTableSize(global_sb), (uint8_t*)it.inodes);
}


size_t getInodeTableSize(Superblock sb)
{
	return roundUp((sb.inodesInBg * sizeof(Inode)), BLOCK_SIZE);
}

Inode getInode(unsigned int inodeIndex, BGD bgd)
{
	Inode inode;
	disk.read(BLOCK_OFFSET(bgd.inodeTable) + (inodeIndex * sizeof(Inode)), sizeof(Inode), (uint8_t*)&inode);
	return inode;
}

void setInode(unsigned int inodeIndex, BGD bgd, Inode inode)
{
	disk.write(BLOCK_OFFSET(bgd.inodeTable) + (inodeIndex * sizeof(Inode)), sizeof(Inode), (uint8_t*)&inode);
}


BGD getFreeGroup()
{
	unsigned int groupCount = (int)(1 + (global_sb.totalBlocks-1) / global_sb.blocksInBg);
	for (int i = 0; i < groupCount; i++)
	{
		BGD bg = global_bgdt.groupDescriptors[i];
		if(bg.freeBlocks >= 1)
			return bg;
	}
	return (BGD){};
}
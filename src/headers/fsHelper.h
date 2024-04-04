#pragma once

#include "std.h"
#include "diskPort.h"
#include "bitmap.h"
#include "std.h"
#include "virtualMemory.h"
#include "ahci.h"
#include "bitMapDS.h"
#include "blockGroups.h"

#define MAX_NAME_LENGTH 64

struct DirectoryEntry // used in disk
{
	uint32_t InodeIdx; // global index of the inode
	uint8_t headerSize;
	uint8_t fileType;
	// uint16_t nameLength; // file name length
	char fileName[MAX_NAME_LENGTH];
}__attribute__((packed));

struct Directory // used in ram
{
	DirectoryEntry entry;
	Directory* next;
}__attribute__((packed));



Inode getInode(unsigned int globalInodeIndex);

// ! do not forget to free memory after use !
size_t readBlockList(uint32_t* blockLists, uint8_t** buffer, size_t size);
// ! do not forget to free memory after use !
uint8_t* getInodeData(Inode inode);

uint32_t getBlockGroupIndex(uint32_t globalInodeIndex);

// ! this function creates linked list that needs to be freed after use (using freeDirectoryList) !
Directory* readDirectoryList(Inode inode);
void freeDirectoryList(Directory* head);

uint32_t getFreeInodeIdx(uint8_t* bitmap);
uint32_t getFreeBlockIdx(uint8_t* bitmap);

// beware this function overwrites old data
bool writeInode(uint8_t* data, size_t size, uint32_t globalInodeIndex);

size_t getInodeSize(Inode inode);

void deleteInode(uint32_t globalInodeIndex);


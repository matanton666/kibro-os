#pragma once

#include "std.h"
#include "serial.h"
#include "ahci.h"
#include "virtualMemory.h"
#include "bitMapDS.h"

#define NUM_DIRECT_POINTERS 12 // number of direct blocks in block list for inode
#define INDIRECT_BLOCK NUM_DIRECT_POINTERS + 1
#define DOUBLY_INDIRECT_BLOCK INDIRECT_BLOCK + 1
#define TRIPLY_INDIRECT_BLOCK DOUBLY_INDIRECT_BLOCK + 1
#define TOTAL_INODE_BLOCKS TRIPLY_INDIRECT_BLOCK + 1


#define SB_SIZE 1024
#define SB_UNUSED 760 // num of bytes
#define SB_POSITION 1024 // the super block is at the 1024 byte from the volume start
#define EXT_MAGIC 0xEF53

#define BLOCK_SIZE 1024 << global_sb.blockSize 
#define DISK_SIZE 50 * 1024 * 1024 // 50MB // FIXME: change this to the actual disk size

#define BLOCK_OFFSET(block) (SB_POSITION + (block-1)*BLOCK_SIZE)
#define REV_BLOCK_OFFSET(offset) ((offset - SB_POSITION) / BLOCK_SIZE + 1)


enum InodeType {FIFO = 0x1000, CD = 0x2000, DIR = 0x4000, BD = 0x6000, RF = 0x8000, SL = 0xA000, US = 0xC000}; //FIFO, Character device, Directory, Block device, Regular file, Symbolic link, Unix socket
enum DirFileType {FT_UNKNOWN, FT_RF, FT_DIR, FT_CD, FT_BD, FT_FIFO, FT_SOCKET, FT_SL}; //Unknown type,Regular file, Directory, Character device, Block device, FIFO, Socket, Symbolic link(soft link)
enum InodePermissions {OTHER_EXE = 0x001, OTHER_WRITE = 0x002, OTHER_READ = 0x004, GROUP_EXE = 0x008, GROUP_WRITE = 0x010, GROUP_READ = 0x20, USER_EXE = 0x40, USER_WRITE = 0x80, USER_READ = 0x100, SB = 0x200, SET_G_ID = 0x400, SET_U_ID = 0x800}; // Sticky Bit, Set group ID, Set user ID
enum InodeFlags {SD = 0x00000001, SAVE_COPY = 0x00000002, COMP = 0x00000004, SYNC = 0x00000008, IMMUTABLE = 0x00000010, APPEND = 0x00000020, DONT_INCLUDE = 0x00000040, DONT_UPDATE_AT = 0x00000080, HASH_IDX_DIR = 0x00010000, AFS_DIR = 0x00020000, JOURNAL_DATA = 0x00040000};
enum CreationErrors {ALREADY_EXISTS = 1, INVALID_PATH, SIZE_ERROR, IO_ERROR};


enum SystemStates {CLEAN = 1, ERROR}; 
enum ErrHandleMethod {IGNORE = 1, REMOUNT, PANIC};
enum OSIds {LINUX = 1, HURD, MASIX, FREE_BSD, LITES};
enum RFeatureFlags {COMP_FLAG = 0x0001, TYPE_FILE = 0x0002, REPLAY_JOURNAL = 0x0004, JOURNAL_DEVICE = 0x0008};
enum ROFeatureFlags {PARSE = 0x0001, SIZE_64 = 0x0002, DIR_BINARY_TREE = 0x0004};


struct Superblock
{
	uint32_t totalInodes;
	uint32_t totalBlocks;
	uint32_t blocksReserved;
	uint32_t totalFreeBlocks;
	uint32_t totalFreeInodes;
	uint32_t sbContainerBlock = 1; // first block after the boot block
	size_t blockSize; // log2 (block size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the block size)
	size_t fragmentSize; // log2(fragment size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the fragment size)
	uint32_t blocksInBg = 8192; // Number of blocks in each block group
	uint32_t fragmentsInBg; // Number of fragments in each block group
	uint32_t inodesInBg = 8192; // Number of inodes in each block group
	uint32_t lastMountTime; // in POSIX time
	uint32_t lastWrittenTime; // in POSIX time
	uint16_t totalMountsSinceFSCK; // Number of times the volume has been mounted since its last consistency check (fsck)
	uint16_t totalMountsAllowedFSCK; // Number of mounts allowed before a consistency check (fsck) must be done
	uint16_t ext2Sign = 0xef53; // 	Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume (magic number)
	uint16_t state = CLEAN;
	uint32_t errorHandle = PANIC;
	uint16_t verMinor;
	uint32_t lastFSCK; // in POSIX time
	uint32_t FSCKIntervals; // in POSIX time
	uint32_t OSID; // creator OS ID
	uint32_t verMajor;
	uint16_t userID; // user ID that can use reserved blocks
	uint16_t groupID; // group ID that can use reserved blocks
	uint32_t firstFreeInode;
	size_t inodeSize : 16;
	uint16_t selfBlockGroup;
	uint32_t optionalFeature; // optional feature present
	uint32_t requiredFeature; // required Feature present
	uint32_t readOnlyFeature; // read only feature - Features that if not supported, the volume must be mounted read-only see below)
	uint64_t FSID[2]; // File system ID - 16 bytes
	char volName[16]; // Volume name - 16 bytes
	char lastMountPath[64]; // Path volume was last mounted to (C-style string: characters terminated by a 0 byte) - 64 bytes
	uint32_t compAlgo; // Compression algorithms used
	uint8_t blockPreallocFiles; // Number of blocks to preallocate for files
	uint8_t blockPreallocDirs; // Number of blocks to preallocate for directories
	uint16_t unused1;
	uint64_t journalID[2]; // journal ID - 16 bytes
	uint32_t journalInode; // journal inode
	uint32_t journalDevice; // journal device
	uint32_t inodeOrphanHead; // Head of orphan inode list
	uint32_t hashSeed[4]; // HTREE hash seed
	uint16_t defHashVersion; // Default hash version
	uint32_t defMountOptions; // Default mount options
	uint32_t firstMetaBlockGroup; // First metablock block group
	uint8_t unused[SB_UNUSED]; // unused - 760 bytes
}__attribute__((packed));


struct BGD // Block Group Descriptor (this goes on the disk)
{
	// each of the entries is the block number that contains the data
	uint32_t blockUsageBitmap; //the block usage bitmap
	uint32_t inodeUsageBitmap; //the inode usage bitmap
	uint32_t inodeTable; // Starting block address of inode table
	uint16_t freeBlocks;
	uint16_t freeInodes;
	uint16_t totalDirs; // Number of directories in group
	uint64_t unused1; // unused 14 bytes
	uint64_t unused2 : 48;
}__attribute__((packed));


struct BGDT // Block Group Descriptor Table (Group Descriptors) (the address is the on the disk and not on ram)
{
	BGD* groupDescriptors; // *length* = (int)(1 + (global_sb.totalBlocks-1) / global_sb.blocksInBg)
}__attribute__((packed));

struct Inode
{
	uint16_t typeAndPermissions;
	uint16_t userID;
	uint32_t lowerFileSize; // Lower 32 bits of file size in bytes
	uint32_t lastAccess; // in POSIX time
	uint32_t creationTime;  // in POSIX time
	uint32_t lastModify; // in POSIX time
	uint32_t deletionTime; // in POSIX time
	uint16_t groupID;
	uint16_t totalHardLinks; // Count of hard links (directory entries) to this inode. When this reaches 0, the data blocks are marked as unallocated.
	uint32_t totalSectors; // Count of disk sectors (not Ext2 blocks) in use by this inode, not counting the actual inode structure nor directory entries linking to the inode.
	uint32_t flags; 
	uint32_t OSSV1; //Operating System Specific value #1
	uint32_t directBlockPtrs[NUM_DIRECT_POINTERS]; 
	uint32_t SIBP; // Singly Indirect Block Pointer (Points to a block that is a list of block pointers to data)
	uint32_t DIBP; // Doubly Indirect Block Pointer (Points to a block that is a list of block pointers to Singly Indirect Blocks)
	uint32_t TIBP; //Triply Indirect Block Pointer(Points to a block that is a list of block pointers to Doubly Indirect Blocks)
	uint32_t genNum; // Generation number (Primarily used for NFS)
	uint32_t EAB; // In Ext2 version 0, this field is reserved. In version >= 1, Extended attribute block (File ACL).
	uint32_t upperFileSize; //In Ext2 version 0, this field is reserved.In version >= 1, Upper 32 bits of file size(if feature bit set) if it's a file, Directory ACL if it's a directory
	uint32_t blockFragmentAddr;// block address of fragment
	uint32_t OSSV2[3]; // Operating System Specific Value #2
}__attribute__((packed));


struct InodeTable // the address is the on the disk and not on ram
{
	Inode* inodes;
}__attribute__((packed));


// this is the structure of a group
// these are only POINTERS to data on DISK, NOT ON RAM! (cannot dereference these pointers)
struct BlockGroup
{
	BGD* bgd;
	uint8_t* dataBlockBitmap; // ptr to the contents of blockUsageBitmap in the BGD // sizeInBytes = BLOCK_SIZE
	uint8_t* inodeBitmap; // ptr to the contents of inodeUsageBitmap in the BGD // sizeInBytes = BLOCK_SIZE
	Inode* inodeTable; // ptr to the contents of inodeTable in the BGD // sizeInBytes = (sb.inodesInBg * sizeof(Inode)) rounded up
	uint8_t* data;
};

// round up a number to mulitple
uint32_t roundUp(uint32_t numToRound, uint32_t multiple);

Superblock createSuperblock();
Superblock getSuperblock();
void setSuperblock(Superblock sb);
BGDT createBGDT();
void setBGDT(BGDT bgdt);

// returns block group of index, the block group contains addresses of structures on the disk
BlockGroup getBlockGroup(unsigned int blockGroupIdx);
BGD getBGD(unsigned int blockGroupIdx);

// dont forget to free memeory after use
uint8_t* getBlockBitmap(BGD bgd);
// dont forget to free memeory after use
uint8_t* getInodeBitmap(BGD bgd);
// dont forget to free memeory after use
InodeTable getInodeTable(BGD bgd);

void setBGD(unsigned int blockGroupIdx, BGD bgd);

void setBlockBitmap(BGD bgd, uint8_t* blockBitmap);
void setInodeBitmap(BGD bgd, uint8_t* inodeBitmap);
void setInodeTable(BGD bgd, InodeTable inodeTable);

size_t getInodeTableSize(Superblock sb);

Inode getInode(unsigned int indoeIndex, BGD bgd);
void setInode(unsigned int inodeIndex, BGD bgd, Inode inode);\

BGD getFreeGroup();

extern Superblock global_sb;
extern BGDT global_bgdt;
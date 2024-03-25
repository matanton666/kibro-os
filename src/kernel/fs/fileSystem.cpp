#include "fileSystem.h"
#define PATH_MAX_LEN 512
Directory* currDir;
uint32_t currDirInodeIdxG = 0; // global index of curr dir inode
const uint32_t rootInodeIdxG = 2; // global index of root inode
const uint32_t fatherInodeIdxG = 1; // g index of the father inode (the one that points to the root directory)
char currPath[PATH_MAX_LEN];

const InodeType dirFileToInodeType[] = {RF, RF, DIR, CD, BD, FIFO, US, SL};

bool initFileSystem()
{
	// Read the superblock from disk
	global_sb = getSuperblock();
	// check if the superblock is valid
	if (global_sb.ext2Sign != EXT_MAGIC)
	{
		createFS();
	}
	else
	{
		// set current directory to root
		currDir = readDirectoryList(getInode(rootInodeIdxG));
		currDirInodeIdxG = rootInodeIdxG;
		currPath[0] = '/';
		currPath[1] = 0;
	}
	
	return true;
}

bool createFS()
{
	// set globals
	global_sb = createSuperblock();
	setSuperblock(global_sb);

	global_bgdt = createBGDT();
	setBGDT(global_bgdt);

	// create inode that will be the "father" inode to the root directory
	Inode rootInode;
	memset(&rootInode, 0, sizeof(Inode));
	rootInode.typeAndPermissions = (USER_WRITE << 12) | InodeType::DIR;

	// set and save inode to disk
	unsigned int blockGroup = 0;
	BGD bgd = getBGD(blockGroup);
	uint8_t* inodeBitmap = getInodeBitmap(bgd);
	BitMapDS::setBit(inodeBitmap, fatherInodeIdxG, fatherInodeIdxG);
	global_sb.totalFreeBlocks--;
	bgd.freeBlocks--;
	setSuperblock(global_sb);
	setInode(fatherInodeIdxG, bgd, rootInode);
	setBGD(blockGroup, bgd);
	setInodeBitmap(bgd, inodeBitmap);
	kernelPaging.getAllocator()->free(inodeBitmap);

	currDirInodeIdxG = fatherInodeIdxG;

	createNewDirectory("/"); // TODO: root should not have .. entry

	currDir = readDirectoryList(getInode(rootInodeIdxG));
	currDirInodeIdxG = rootInodeIdxG;
	currPath[0] = '/';
	currPath[1] = 0;

	createNewDirectory("user1");
	cd("user1");
	// createNewDirectory("Documents");
	// createNewDirectory("Desktop");
	// createNewDirectory("Downloads");
	// createNewDirectory("Pictures");
	// cd("..");
	return true;
}

DirectoryEntry createDirEntry(char* name, DirFileType type)
{
	DirectoryEntry DE = {0};

	if (strlen(name) >= MAX_NAME_LENGTH)
		return DE;

	DE.fileType = type;
	strcpy(DE.fileName, name);


	Inode inode;
	memset(&inode, 0, sizeof(Inode));
    inode.typeAndPermissions = (USER_WRITE << 12) | dirFileToInodeType[type];
	
	// check if current block that holds the directory entries is full
	unsigned int blockGroupNum = (currDirInodeIdxG - 1) / global_sb.inodesInBg;
	BGD bgd = getBGD(blockGroupNum);
	uint8_t* inodeBitmap = getInodeBitmap(bgd);
	uint32_t inodeIndex = getFreeInodeIdx(inodeBitmap); 
	
	// if (inodeIndex == SIZE_ERROR) // FIXME: change SIZE_ERROR to a bigger number (not 3)
	// 	return SIZE_ERROR;

	// set index to global
	DE.InodeIdx = inodeIndex + blockGroupNum * global_sb.inodesInBg;
	

	// set and save inode to disk
	setInode(inodeIndex, bgd, inode);
	BitMapDS::setBit(inodeBitmap, inodeIndex, 1);// set used in bitmap
	global_sb.totalFreeBlocks--;
	bgd.freeBlocks--;

	setSuperblock(global_sb);
	setBGD(blockGroupNum, bgd);
	setInodeBitmap(bgd, inodeBitmap);
	kernelPaging.getAllocator()->free(inodeBitmap);

	return DE;
}

int addDirEntryToPath(DirectoryEntry entry)
{
	// add DE to the current directory on disk
	uint32_t size = 0;
	Directory* newDir = (Directory *)kernelPaging.getAllocator()->malloc(sizeof(Directory));

	// get size of linked list and push new directory to the end
	if (currDir == nullptr)
	{
		currDir = newDir;
	}
	else
	{
		Directory *temp = currDir;
		while (temp->next != nullptr)
		{
			temp = temp->next;
			// check if directory already has that entry
			if (memcmp(temp->entry.fileName, entry.fileName, strlen(entry.fileName)) == 0)
			{
				write_serial("file already exists:");
				write_serial(entry.fileName);
				return ALREADY_EXISTS;
			}
			// offset + sizeof(DirectoryEntry) - sizeof(char*) + nameLentth
			size += sizeof(DirectoryEntry);
		}
		size += sizeof(DirectoryEntry);// account for last entry (not including the new entry to add)
		temp->next = newDir;
	}

	newDir->entry = entry;
	newDir->next = nullptr;
	size += sizeof(DirectoryEntry); // account for the new entry to add

	// update directory on disk
	uint8_t* data = (uint8_t *)kernelPaging.getAllocator()->calloc(size);
	// copy Directory to data
	Directory* temp = currDir;
	uint32_t offset = 0;
	while (temp != nullptr)
	{
		memcpy(data + offset, &temp->entry, sizeof(DirectoryEntry));
		offset += sizeof(DirectoryEntry);
		temp = temp->next;
	}
	
	writeInode(data, size, currDirInodeIdxG);
	kernelPaging.getAllocator()->free(data);
	return 0;
}

int createNewDirectory(char* name)
{
	DirectoryEntry newDir = createDirEntry(name, DirFileType::FT_DIR);
	if (newDir.InodeIdx == 0)
		return -1;
	
	addDirEntryToPath(newDir);

	// move temporaraly to the new directory to create the . and .. entries
	Directory* parentDir = currDir;
	uint32_t parentInodeIdx = currDirInodeIdxG;
	currDir = nullptr;
	currDirInodeIdxG = newDir.InodeIdx;


	DirectoryEntry dotFile = { newDir.InodeIdx, 0, DirFileType::FT_DIR, "." };
	DirectoryEntry dotDotFile = { parentInodeIdx, 0, DirFileType::FT_DIR, ".." };

	if (addDirEntryToPath(dotFile) != 0 || addDirEntryToPath(dotDotFile) != 0)
	{
		write_serial("failed to create . and .. entries");
		// move back to the parent directory
		currDir = parentDir;
		currDirInodeIdxG = parentInodeIdx;
		return -1;
	
	}

	// move back to the parent directory
	currDir = parentDir;
	currDirInodeIdxG = parentInodeIdx;
	return 0;
}


size_t splitPath(char* str, char delimiter, char** tokens)
{
	unsigned int wordCount = 0;
	char* wordStart = str;

	tokens[wordCount] = nullptr; // Initialize the first token as nullptr

	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] == delimiter)
		{
			str[i] = 0;
			tokens[wordCount] = wordStart;
			wordCount++;
			wordStart = &(str[i + 1]);
		}
	}
	if (wordStart != 0)
	{
		tokens[wordCount] = wordStart;
		wordCount++;
	}

	tokens[wordCount] = nullptr; // Initialize the last token as nullptr

	return wordCount;
}

Directory* getDirectory(char* name)
{
	return readDirectoryList(getInode(getDirEntry(name).InodeIdx));
}

DirectoryEntry getDirEntry(char* name)
{
	if (strcmp(name, "/") == 0)
	{
		uint8_t* data = getInodeData(getInode(fatherInodeIdxG));
		DirectoryEntry entry;
		memcpy(&entry, data, sizeof(DirectoryEntry));
		kernelPaging.getAllocator()->free(data);
		return entry;
	}
	
	Inode inode = getInode(currDirInodeIdxG);
	Directory* dirList = readDirectoryList(inode); 

	if (dirList == nullptr)
	{
		write_serial("could not read the current directory list");
		return DirectoryEntry {};
	}

	Directory* temp = dirList;
	while (temp != nullptr)
	{
		if (strcmp(temp->entry.fileName, name) == 0)
		{
			DirectoryEntry entry = temp->entry;
			freeDirectoryList(dirList);
			return entry;
		}
		temp = temp->next;
	}

	write_serial("could not find the directory entry specifyed");
	return DirectoryEntry {};
}


bool cd(char* name)
{
	DirectoryEntry newDir = getDirEntry(name);
	if (newDir.InodeIdx == 0 || newDir.fileType != DirFileType::FT_DIR)
	{
		write_serial("could not change directory");
		return false;
	}

	freeDirectoryList(currDir);
	Directory* dir = readDirectoryList(getInode(newDir.InodeIdx));

	currDir = dir;
	currDirInodeIdxG = newDir.InodeIdx;

	// update current tempPath
	if (strcmp(name, "/") == 0 || currDirInodeIdxG == rootInodeIdxG)
	{
		currPath[0] = '/';
		currPath[1] = 0;
	}
	else if (strcmp(name, ".") == 0)
	{
	}
	else if (strcmp(name, "..") == 0)
	{
		currPath[strlen(currPath) - 1] = 0; // remove the last '/'
		for (int i = strlen(currPath) - 1; i >= 0; i--)
		{
			if (currPath[i] == '/')
			{
				currPath[i+1] = 0;
				break;
			}
		}
	}
	else
	{
		strcat(currPath, name); 
		strcat(currPath, "/");
	}

	return true;
}


int writeToFile(char* name, uint8_t* data, size_t size)
{
	DirectoryEntry fileEntry = getDirEntry(name);
	if (fileEntry.InodeIdx == 0)
	{
		write_serial("could not write to file because it does not exist");
		write_serial(name);
		return -1;
	}

	// BGD bgd = getBGD(getBlockGroupIndex(fileEntry.InodeIdx));
	return writeInode(data, size, fileEntry.InodeIdx) ? size : 0;
}

int readFromFile(char* name, uint8_t* data, size_t size)
{
	Inode fileInode = getInode(name);

	uint8_t* tempBuf = getInodeData(fileInode);
	if (tempBuf == nullptr)
	{
		write_serial("could not read the file");
		return -1;
	}

	memcpy(data, tempBuf, size);
	kernelPaging.getAllocator()->free(tempBuf);

	return 0;
}

int appendToFile(char* name, uint8_t* data, size_t size)
{
	size_t oldSize = getFileSize(name);
	uint8_t* mergedData = (uint8_t *)kernelPaging.getAllocator()->malloc(oldSize + size);

	readFromFile(name, data, oldSize); // read old data
	memcpy(mergedData + oldSize, data, size);
	writeToFile(name, mergedData, oldSize + size);

	kernelPaging.getAllocator()->free(mergedData);
	return 0;
}

size_t getFileSize(char* name)
{
	Inode fileInode = getInode(name);
	return getInodeSize(fileInode);
}

bool deleteFile(char* name)
{
	DirectoryEntry fileEntry = getDirEntry(name);
	if (fileEntry.InodeIdx == 0)
	{
		write_serial("could not delete file because it does not exist");
		write_serial(name);
		return false;
	}

	deleteInode(fileEntry.InodeIdx);
	return true;
}

bool deleteDir(char* name)
{
	DirectoryEntry dirEntry = getDirEntry(name);
	if (dirEntry.InodeIdx == 0)
	{
		write_serial("could not delete directory because it does not exist");
		write_serial(name);
		return false;
	}

	Directory* dir = readDirectoryList(getInode(dirEntry.InodeIdx));
	if (dir == nullptr)
	{
		write_serial("could not read the directory list");
		return false;
	}

	cd(name); // move to the directory to delete
	// recursively delete all files and directories in the directory
	Directory* temp = dir;

	// skip the . and .. entries
	temp = temp->next->next;

	// recursively delete all files and directories in the directory
	while (temp != nullptr)
	{
		if (temp->entry.fileType == DirFileType::FT_DIR)
		{
			deleteDir(temp->entry.fileName);
		}
		else
		{
			deleteFile(temp->entry.fileName);
		}
		temp = temp->next;
	}

	// deleteFile("."); // TODO: test if this is correct
	cd(".."); // move back to the parent directory

	deleteInode(dirEntry.InodeIdx);
	return true;
}

Inode getInode(char* name)
{
	DirectoryEntry entry = getDirEntry(name);
	return getInode(entry.InodeIdx);
}

char* getCurrPath()
{
	return currPath;
}

void updateCurrDir()
{
	freeDirectoryList(currDir);
	currDir = readDirectoryList(getInode(currDirInodeIdxG));
}

Directory* getCurrentDir()
{
	updateCurrDir();
	return currDir;
}
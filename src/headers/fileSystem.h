#pragma once

#include "fsHelper.h"


bool initFileSystem();
bool createFS();

bool cd(char* name);

bool deleteFile(char* name);
int writeToFile(char* name, uint8_t* data, size_t size);
int readFromFile(char* name, uint8_t* data, size_t size);
int appendToFile(char* name, uint8_t* data, size_t size);

size_t getFileSize(char* name);

bool deleteDir(char* name);
int createNewDirectory(char* name);

Directory* getCurrentDir();

// save directory entry to disk in the path the inode points to 
int addDirEntryToPath(DirectoryEntry entry);
int removeDirEntryFromPath(char* name);
DirectoryEntry createDirEntry(char* name, DirFileType type);

// recursive function to get the dir for a path (beware that if it is a file it will fail)
// Directory* getDir(Directory* dir, char** tokens, int idx, int finish);

// returns the number of tokens (non-zero indexing)
size_t splitPath(char *str, char delimiter, char** tokens);

// ! dont forget to use freeDirectoryList to free the memory !
Directory* getDirectory(char* name);
DirectoryEntry getDirEntry(char* name);

Inode getInode(char* name);

char* getCurrPath();

void updateCurrDir();


#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"

const char FLAG_FILE = 'f';
const char FLAG_DIRECTORY = 'd';

struct FileInfo {
	bool exist;
	std::string fileName;
	char flag;
	int blockIndex;
	int parrentBlockIndex;
};

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;

	unsigned int mRootStart;
	//unsigned int mBlockCount;

    // Here you can add your own data structures
	const int elementNameSize = 13;
	const int nodeInfo = 3;
	const int rowSize = nodeInfo + elementNameSize;


public:
    FileSystem();
    ~FileSystem();

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

	//Check if File/Folder Exist
	FileInfo Exist(std::string path, int startBlock = -1);

    /* This function creates a file or Folder in the filesystem. startBlock = -1 meens root, Returns Block id of created file( -1 if failed )*/
	int Create(std::string fileName, char flag, int startBlock = -1);

    /* Removes a file or Folder in the filesystem */
    bool Remove(std::string fileName, int startBlock = -1);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

	std::string readFile(std::string path, int startBlock = -1);

    /* This function will get all the files and folders in the specified folder */
    std::string listDir(std::string path, int startBlock = -1);

	/* Gangbang at the old folks home */
	std::string GetStringContainingAllBlocks();

	// this is currently only for one block at a time.
	void RestoreImage(int i, char* data);

    /* Add your own member-functions if needed */
	int freeSpace();
};

#endif // FILESYSTEM_H

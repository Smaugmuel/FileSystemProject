#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"

const char FLAG_FILE = 'f';
const char FLAG_DIRECTORY = 'd';

struct FileInfo {
	bool exist;
	std::string fileName;
	char flag;
	unsigned int blockIndex;
	unsigned int parrentBlockIndex;
};

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;

	unsigned int mRootStart;
	//unsigned int mBlockCount;

    // Here you can add your own data structures
	
	const int NodeFlagSize = 1;
	const int NodeBlockIndexSize = 2;
	const int NodeElementSizeSize = 4;

	const int NodeElementInfo = NodeFlagSize + NodeBlockIndexSize + NodeElementSizeSize;//Sum Of All Above
	const int NodeElementNameSize = 25;
	
	const int rowSize = NodeElementInfo + NodeElementNameSize;

	const int nextBlockInfoSize = 12;


public:
    FileSystem();
    ~FileSystem();

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

	//Check if File/Folder Exist
	FileInfo Exist(std::string path, int startBlock = -1);

    /* This function creates a file or Folder in the filesystem. startBlock = -1 means root, Returns Block id of created file( -1 if failed )*/
	int Create(std::string fileName, char flag, int startBlock = -1);

    /* Removes a file or Folder in the filesystem */
    bool Remove(std::string fileName, int startBlock = -1);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

	int WriteFile(std::string data, std::string path, unsigned int offset = 0, int startBlock = -1);
	int AppendFile(std::string data, std::string path, int startBlock = -1);

	std::string readFile(std::string path, int startBlock = -1);

	bool CopyFile(std::string oldFilePath, std::string newFilePath);

	bool MoveFile(std::string oldFilePath, std::string newFilePath, int startBlock = -1);

    /* This function will get all the files and folders in the specified folder */
    std::string listDir(std::string path, int startBlock = -1);

    /* Add your own member-functions if needed */
	int freeSpace();
};

#endif // FILESYSTEM_H

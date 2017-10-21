#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"

const char FLAG_FILE = 'f';
const char FLAG_DIRECTORY = 'd';

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;

	unsigned int mRootStart;
	//unsigned int mBlockCount;

    // Here you can add your own data structures

public:
    FileSystem();
    ~FileSystem();

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

    /* This function creates a file in the filesystem */
    // createFile(...)
	bool Create(std::string fileName, char flag);

    /* Creates a folder in the filesystem */

    /* Removes a file in the filesystem */
    // removeFile(...);

    /* Removes a folder in the filesystem */
    // removeFolder(...);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

    /* This function will get all the files and folders in the specified folder */
    // listDir(...);

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H

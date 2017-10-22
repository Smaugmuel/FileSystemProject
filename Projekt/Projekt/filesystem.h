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

	/*Info how to structure Folders (I-Nodes)*/
	/*

	// -> means - "same as before"
	Byte Use: [Flag][Block#][->][FileSize][->][->][->][Owner][->][->][->][->][->][->][->][AccessRights][->][FileName][->][->][->][->][->][->][->][->][->][->][->][->][->][->]
	Byte ID:  [0]  [1]      [2] [3]       [4] [5] [6] [7]    [8] [9] [10][11][12][13][14][15]          [16][17][18][19]      [20][21][22][23][24][25][26][27][28][29][30][31]

	*/
	const int SIZE_NodeFlag = 1, INDEX_NodeFlag = 0;//File Flag, Tells if a 'file' is a directory of file
	const int SIZE_NodeBlockIndex = 2, INDEX_NodeBlockIndex = INDEX_NodeFlag + SIZE_NodeFlag;//Block index, where the file is stored on disk
	const int SIZE_NodeFileSize = 4, INDEX_NodeFileSize = INDEX_NodeBlockIndex + SIZE_NodeBlockIndex;//FileSize
	const int SIZE_NodeOwnerID = 2, INDEX_NodeOwnerName = INDEX_NodeFileSize + SIZE_NodeFileSize;//OwnerName
	const int SIZE_NodeAccesRights = 2, INDEX_NodeAccesRight = INDEX_NodeOwnerName + SIZE_NodeOwnerID;//AccesRights
	const int SIZE_NodeFileName = 21, INDEX_NodeFileName = SIZE_NodeAccesRights + INDEX_NodeAccesRight;//FileName
	
	//Row is 32 bytes
	const int rowSize = INDEX_NodeFileName + SIZE_NodeFileName;//BlockSize(512) must be divideable with This number,

	const int BLOCKSIZE = 512;

	/*Info how to blocks containing file data*/
	const int SIZE_FILEBLOCK_nextBlockInfo = 12, INDEX_FILEBLOCK_nextBlockInfo = BLOCKSIZE - SIZE_FILEBLOCK_nextBlockInfo;
	const int SIZE_FILEBLOCK_ACUAL_FILE_DATA = BLOCKSIZE - SIZE_FILEBLOCK_nextBlockInfo;

	void init();

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

	int CopyFile(std::string oldFilePath, std::string newFilePath);

	int MoveFile(std::string oldFilePath, std::string newFilePath, int startBlock = -1);

    /* This function will get all the files and folders in the specified folder */
    std::string listDir(std::string path, int startBlock = -1);

	void FormatDisk();

    /* Add your own member-functions if needed */
	int freeSpace();
};

#endif // FILESYSTEM_H

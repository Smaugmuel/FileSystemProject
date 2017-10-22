#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileInfo FileSystem::Exist(std::string path, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo f = { false, "" ,FLAG_FILE, -1, -1 };

	// Replace spaces with underlines and slashes with spaces
	// to utilize stringstream easily
	std::replace(path.begin(), path.end(), ' ', '_');
	std::replace(path.begin(), path.end(), '/', ' ');

	// Rows consist of 2 bytes for information and 14 for the name

	//===============================================================================
	// Create an array of the path's folder names
	std::vector<std::string> folders;
	std::string name;
	std::stringstream ss(path);
	while (ss >> name)
	{
		// Shorten name until a maximum certain length
		while (name.size() > SIZE_NodeFileName)
		{
			name.pop_back();
		}

		folders.push_back(name);
	}

	//===============================================================================
	// Read from the root block
	unsigned int block = (unsigned int)startBlock;

	std::string b = mMemblockDevice.readBlock(block).toString();
	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;

	//If file is not created in current directory
	for (int i = 0; i < folders.size(); i++)
	{
		int row = 0;
		bool found = false;
		char a;
		while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows && !found)
		{
			std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			nameAtRow.size();

			if (nameAtRow.compare(folders.at(i)) == 0) {//Folder Exist!

				if (i != folders.size() - 1) {
					if (b[row*rowSize + INDEX_NodeFlag] != FLAG_DIRECTORY) {//Something exist with correct name but it is not a directory. Process can not continue!

						return f;
					}
					else {
						block = (unsigned int)int(b[row*rowSize + INDEX_NodeBlockIndex] << 8 | b[row*rowSize + INDEX_NodeBlockIndex + 1]);//Switch Block nr to the next Folder in pathname.
						found = true;
					}
				}
				else {
					f.exist = true;
					f.fileName = folders.at(i);
					f.flag = b[row*rowSize];
					f.blockIndex = (unsigned int)int(b[row*rowSize + INDEX_NodeBlockIndex] << 8 | b[row*rowSize + INDEX_NodeBlockIndex + 1]);//TODO: unsigned char
					f.parrentBlockIndex = block;

					return f;
				}
			}

			row++;
		}

		if (found) {//Folder Exist And The Process can continue
			b = mMemblockDevice.readBlock(block).toString();//Read Data from next block
		}
		else {//Folder Does'nt Exist and needs to be created before creating the file inside it!!!!!!!!!!!!!!!!!!!!!
			return f;
		}
	}

	return f;
}

void FileSystem::init()
{
	mRootStart = mMemblockDevice.reservBlock();//TODO:: ERROR hadeling if reservBlock() returns -1

	std::string b = mMemblockDevice.readBlock(mRootStart).toString();

	//Add folder "." pointing to itself
	b[INDEX_NodeFlag] = FLAG_DIRECTORY;
	b[INDEX_NodeBlockIndex] = (unsigned char)(((unsigned int)(mRootStart) >> 8) & 0xFF);//
	b[INDEX_NodeBlockIndex+1] = (unsigned char)((unsigned int)(mRootStart) & 0xFF);//

	b.replace(b.begin() + (0 * rowSize + INDEX_NodeFileName), b.begin() + (0 * rowSize + INDEX_NodeFileName + 1), ".");

	//Add folder ".." pointing to parrent(itself becuse root)
	b[1 * rowSize + INDEX_NodeFlag] = FLAG_DIRECTORY;
	b[1 * rowSize + INDEX_NodeBlockIndex] = (unsigned char)(((unsigned int)(mRootStart) >> 8) & 0xFF);//
	b[1 * rowSize + INDEX_NodeBlockIndex + 1] = (unsigned char)((unsigned int)(mRootStart) & 0xFF);//

	b.replace(b.begin() + (1 * rowSize + INDEX_NodeFileName), b.begin() + (1 * rowSize + INDEX_NodeFileName + 2), "..");

	//Rewrite root block
	mMemblockDevice.writeBlock(mRootStart, b);
}

FileSystem::FileSystem() {
	init();
}

FileSystem::~FileSystem() {

}

int FileSystem::Create(std::string fileName, char flag, int startBlock)
{

	// Replace spaces with underlines and slashes with spaces
	// to utilize stringstream easily
	std::replace(fileName.begin(), fileName.end(), ' ', '_');
	std::replace(fileName.begin(), fileName.end(), '/', ' ');

	// Rows consist of 2 bytes for information and 14 for the name

	//===============================================================================
	// Create an array of the path's folder names
	std::vector<std::string> folders;
	std::string name;
	std::stringstream ss(fileName);
	while (ss >> name)
	{
		// Shorten name until a maximum certain length
		while (name.size() > SIZE_NodeFileName)
		{
			name.pop_back();
		}

		folders.push_back(name);
	}

	// The last name is the file name (currently stored in 'name'), not a folder
	folders.pop_back();

	//===============================================================================
	unsigned int block;
	if (startBlock == -1) {
		block = mRootStart;
	}
	else
	{
		block = startBlock;
	}

	std::string b = mMemblockDevice.readBlock(block).toString();
	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;


	//If file is not created in current directory
	for (int i = 0; i < folders.size(); i++)
	{
		//Check if Folder Exist
		FileInfo fi = Exist(folders.at(i), block);

		if (fi.exist && fi.flag == FLAG_DIRECTORY) {//If Name Exist and is Directory
			block = fi.blockIndex;//Block id if found directory
			b = mMemblockDevice.readBlock(block).toString();//Read Data from next block
		}
		else if (fi.exist && fi.flag == FLAG_FILE) {
			return -1;//Path no allowed, file have the same name as one of the directorys needed in the path.
		}
		else {
			block = Create(folders.at(i), FLAG_DIRECTORY, block); //Folder in path did'nt exist, so we create it on the fly inside current directory.
			if (block == -1)
				return -1;
			b = mMemblockDevice.readBlock(block).toString();//Read Data from next block
		}
	}

	//===============================================================================
	//Create File in current directory
	//Loop Through all files in last directory(block)
	int row = 0;
	bool found = false;
	char a;
	while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows && !found)
	{
		std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
		nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

		//nameAtRow.size();

		if (nameAtRow.compare(name) == 0) {//File Exist!
			block = (unsigned int)(unsigned char)(b[row*rowSize + 1]) << 8 | (unsigned char)(b[row*rowSize + 2]);
			found = true;
		}

		row++;
	}

	//===============================================================================
	if (found) {//File Exist And Cannot be created
		return -1;
	}
	else {

		// f: Flag for a file
		// 1: Which block it's located in
		std::string data = b;

		unsigned int blockIndex = mMemblockDevice.reservBlock();
		if (blockIndex == -1) //-1 if no free block could be reserved for the file/folder
			return false;

		data[row*rowSize + INDEX_NodeFlag] = flag;
		data[row*rowSize + INDEX_NodeBlockIndex] = (unsigned char)((blockIndex >> 8) & 0xFF);
		data[row*rowSize + INDEX_NodeBlockIndex + 1] = (unsigned char)((blockIndex) & 0xFF);

		data.replace(data.begin() + (row*rowSize + INDEX_NodeFileName), data.begin() + (row*rowSize + INDEX_NodeFileName + name.size()), name);

		mMemblockDevice.writeBlock(block, data);

		//===========================
		//Reset and Config new data block
		b = mMemblockDevice.readBlock(blockIndex).toString();
		b.replace(b.begin(), b.end(), blockSize, '\0');
		if (flag == FLAG_DIRECTORY) {
			//Add folder "." pointing to itself
			b[INDEX_NodeFlag] = FLAG_DIRECTORY;
			b[INDEX_NodeBlockIndex] = (unsigned char)((blockIndex >> 8) & 0xFF);
			b[INDEX_NodeBlockIndex+1] = (unsigned char)((blockIndex) & 0xFF);

			b.replace(b.begin() + (0 * rowSize + INDEX_NodeFileName), b.begin() + (0 * rowSize + INDEX_NodeFileName + 1), ".");

			//Add folder ".." pointing to parrent
			b[1 * rowSize + INDEX_NodeFlag] = FLAG_DIRECTORY;

			b[1 * rowSize + INDEX_NodeBlockIndex] = (unsigned char)((block >> 8) & 0xFF);
			b[1 * rowSize + INDEX_NodeBlockIndex + 1] = (unsigned char)((block) & 0xFF);

			b.replace(b.begin() + (1 * rowSize + INDEX_NodeFileName), b.begin() + (1 * rowSize + INDEX_NodeFileName + 2), "..");
		}

		//Rewrite new block
		mMemblockDevice.writeBlock(blockIndex, b);


		return blockIndex;//Return block index of created file/folder
	}

}

bool FileSystem::Remove(std::string fileName, int startBlock)
{
	if (startBlock == -1) {
		startBlock == mRootStart;
	}

	FileInfo fi = Exist(fileName, startBlock);

	if (fi.exist && fi.flag == FLAG_FILE) {
		int parentBlock = fi.parrentBlockIndex;
		int block = fi.blockIndex;

		std::string b = mMemblockDevice.readBlock(parentBlock).toString();
		int blockSize = b.size(); //MaxBytes
		const int maxRows = blockSize / rowSize;

		int rowToRemove = 2;
		int row = 2;

		bool found = false;
		char a;
		while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows)
		{
			std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			if (nameAtRow.compare(fi.fileName) == 0) {//File Exist!
				rowToRemove = row;
			}

			row++;
		}

		//Replace the "file to be removed"'s row in parrent to the last row in parrent
		b.replace(b.begin() + (rowToRemove*rowSize), b.begin() + (rowToRemove*rowSize + rowSize), b.substr((row - 1)*rowSize, rowSize));
		//Clear last Row in Parrent
		b.replace(b.begin() + ((row - 1)*rowSize), b.begin() + ((row - 1)*rowSize + rowSize), rowSize, '\0');

		mMemblockDevice.freeBlock(block);//Mark The File's Block as free, to free up the space
		mMemblockDevice.writeBlock(parentBlock, b);//rewrite parrent folder block
	}
	else if (fi.exist && fi.flag == FLAG_DIRECTORY) {

		std::string folderItems = mMemblockDevice.readBlock(fi.blockIndex).toString();
		int blockSize = folderItems.size(); //MaxBytes
		const int maxRows = blockSize / rowSize;

		//Start at row 2, Becuse Folder "." and ".." always exist inside a folder and shold not be removed(becuse of loops).
		int row = 2;
		char a;
		bool result = true;

		while ((a = folderItems[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows)
		{
			std::string nameAtRow = folderItems.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			//Recursively remove all files in folder but is ineffective, will result in alot of read/write duplicates but should do the job.
			if (!Remove(nameAtRow, fi.blockIndex))
				result = false;

			row++;
		}

		if (result) {//if all files in folder was removed, folder is safe to remove
			int parentBlock = fi.parrentBlockIndex;
			int block = fi.blockIndex;

			std::string b = mMemblockDevice.readBlock(parentBlock).toString();
			int blockSize = b.size(); //MaxBytes
			const int maxRows = blockSize / rowSize;

			int rowToRemove = 2;
			int row = 2;

			bool found = false;
			char a;
			while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows)
			{
				std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
				nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

				if (nameAtRow.compare(fi.fileName) == 0) {//File Exist!
					rowToRemove = row;
				}

				row++;
			}

			//Replace the "file to be removed"'s row in parrent to the last row in parrent
			b.replace(b.begin() + (rowToRemove*rowSize), b.begin() + (rowToRemove*rowSize + rowSize), b.substr((row - 1)*rowSize, rowSize));
			//Clear last Row in Parrent
			b.replace(b.begin() + ((row - 1)*rowSize), b.begin() + ((row - 1)*rowSize + rowSize), rowSize, '\0');

			mMemblockDevice.freeBlock(block);//Mark The File's Block as free, to free up the space
			mMemblockDevice.writeBlock(parentBlock, b);//rewrite parrent folder block
		}

		return result;
	}
	else {
		return false;
	}

	return true;
}

int FileSystem::WriteFile(std::string data, std::string path, unsigned int offset, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo fi = Exist(path, startBlock);

	std::vector<std::string> blockData;

	int skipBlocks = offset / (SIZE_FILEBLOCK_ACUAL_FILE_DATA);//Calculate how many blocks to skip before writing
	int skipRest = offset % (SIZE_FILEBLOCK_ACUAL_FILE_DATA);//How Far In on the block should it jump before writing

	int size = data.size() + skipRest + skipBlocks*SIZE_FILEBLOCK_ACUAL_FILE_DATA;//Size Of Data beeing written(index of last character beeing written)

	int elements = size / (SIZE_FILEBLOCK_ACUAL_FILE_DATA);
	int rest = size % (SIZE_FILEBLOCK_ACUAL_FILE_DATA);

	int blocksNeeded = elements + (rest == 0 ? 0 : 1);//This many blocks will be needed to fit all data beeing written, after offset

	int dataWritten = 0;//how much data have been written this call

	if (fi.exist /*&& fi.flag == FLAG_FILE*/)
	{
		std::string content;
		unsigned int nextBlock = 0;
		unsigned int currentBlock = fi.blockIndex;

		/*Skip Blocks*/
		for (int i = 0; i < skipBlocks; i++)
		{
			content = mMemblockDevice.readBlock(currentBlock).toString();
			if (content[INDEX_FILEBLOCK_nextBlockInfo] != '\0') {
				nextBlock = (unsigned char)(content[BLOCKSIZE - 2]) << 8 | (unsigned char)(content[BLOCKSIZE - 1]);//Pointer to next file block
			}
			else {
				/*If suposed to skip blocks, but next block dont exist, Create one*/
				nextBlock = mMemblockDevice.reservBlock();

				//dataWritten += SIZE_FILEBLOCK_ACUAL_FILE_DATA;

				if (nextBlock == -1)
					return -1;

				mMemblockDevice[nextBlock].reset();

				content[INDEX_FILEBLOCK_nextBlockInfo] = FLAG_DIRECTORY;//A Flag set if the block continues to another block
				content[BLOCKSIZE - 2] = (unsigned char)((nextBlock >> 8) & 0xFF);//ID to next block
				content[BLOCKSIZE - 2] = (unsigned char)((nextBlock)& 0xFF);//ID to next block
				mMemblockDevice.writeBlock(currentBlock, content);
			}
		}

		while (blocksNeeded-- > 0)
		{
			content = mMemblockDevice.readBlock(currentBlock).toString();

			if (blocksNeeded == 0) {
				std::string sub = data.substr(dataWritten, data.size());

				content.replace(content.begin() + skipRest, content.begin() + (skipRest + sub.size()), sub.c_str());
				dataWritten += skipRest + data.size() - dataWritten;
			}
			else {
				std::string sub = data.substr(dataWritten, SIZE_FILEBLOCK_ACUAL_FILE_DATA - skipRest);

				content.replace(content.begin() + skipRest, content.begin() + SIZE_FILEBLOCK_ACUAL_FILE_DATA, sub.c_str());
				dataWritten += SIZE_FILEBLOCK_ACUAL_FILE_DATA - skipRest;

				if (content[INDEX_FILEBLOCK_nextBlockInfo] != '\0') {
					nextBlock = (unsigned char)(content[BLOCKSIZE - 2]) << 8 | (unsigned char)(content[BLOCKSIZE - 1]);
				}
				else {
					nextBlock = mMemblockDevice.reservBlock();

					if (nextBlock == -1)
						return -1;

					mMemblockDevice[nextBlock].reset();

					content[INDEX_FILEBLOCK_nextBlockInfo] = FLAG_DIRECTORY;//A Flag set if the block continues to another block
					content[BLOCKSIZE - 2] = (unsigned char)((nextBlock >> 8) & 0xFF);//ID to next block
					content[BLOCKSIZE - 1] = (unsigned char)((nextBlock)& 0xFF);//ID to next block
				}
			}

			mMemblockDevice.writeBlock(currentBlock, content);
			currentBlock = nextBlock;
			skipRest = 0;
		}

		/*=====================================================*/
		const int maxRows = BLOCKSIZE / rowSize;

		std::string b = mMemblockDevice.readBlock(fi.parrentBlockIndex).toString();

		int row = 0;
		bool found = false;
		char a;
		while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows && !found)
		{
			std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			//nameAtRow.size();

			if (nameAtRow.compare(fi.fileName) == 0) {//File Exist!

				int lastsize = (unsigned char)(b[row*rowSize + INDEX_NodeFileSize]) << 24 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 1]) << 16 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 2]) << 8 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 3]);

				if (lastsize < size) {//If Last Size is less then size of all written data + offset, Update fileSize
					b[row*rowSize + INDEX_NodeFileSize] = (unsigned char)(((unsigned int)size) >> 24 & 0xFF);
					b[row*rowSize + INDEX_NodeFileSize + 1] = (unsigned char)(((unsigned int)size) >> 16 & 0xFF);
					b[row*rowSize + INDEX_NodeFileSize + 2] = (unsigned char)(((unsigned int)size) >> 8 & 0xFF);
					b[row*rowSize + INDEX_NodeFileSize + 3] = (unsigned char)(((unsigned int)size) & 0xFF);

					mMemblockDevice.writeBlock(fi.parrentBlockIndex, b);
				}

				found = true;
			}

			row++;
		}

		// Succes
		return 1;
	}

	// File didn't exist
	return -2;
}

int FileSystem::AppendFile(std::string data, std::string path, int startBlock)
{
	FileInfo fi = Exist(path, startBlock);

	std::string b = mMemblockDevice.readBlock(fi.parrentBlockIndex).toString();

	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;
	int row = 0;
	bool found = false;
	char a;
	while ((a = b[row*rowSize]) != 0 && row < maxRows && !found)
	{
		std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
		nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

		//nameAtRow.size();

		if (nameAtRow.compare(fi.fileName) == 0) {//File Exist!

			unsigned int fileSize = (unsigned char)(b[row*rowSize + INDEX_NodeFileSize]) >> 24 & 0xFF | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 1]) >> 16 & 0xFF | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 2]) >> 8 & 0xFF | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 3]) & 0xFF;

			//Write to file with fileSize as offset
			return WriteFile(data,path, fileSize,fi.parrentBlockIndex);

			found = true;
		}

		row++;
	}

	return 0;
}

std::string FileSystem::readFile(std::string path, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo fi = Exist(path, startBlock);

	std::string output = "";
	std::string blockData;
	if (fi.exist /*&& fi.flag == FLAG_FILE*/) {

		bool read = true;
		unsigned int block = fi.blockIndex;

		while (read)
		{
			blockData = mMemblockDevice.readBlock(block).toString();
			
			output += blockData.substr(0, std::min(INDEX_FILEBLOCK_nextBlockInfo, (int)blockData.find_last_not_of('\0')+1));

			if (blockData[INDEX_FILEBLOCK_nextBlockInfo] != '\0') {//If file extends to another block, start reading from that block
				block = (unsigned char)(blockData[BLOCKSIZE - 2]) << 8 | (unsigned char)(blockData[BLOCKSIZE - 1]);
			}
			else {
				read = false;
			}
		}
	}

	//output = output.substr(0, output.find('\0'));
	return output;
}

bool FileSystem::CopyFile(std::string oldFilePath, std::string newFilePath)
{
	FileInfo fi = Exist(oldFilePath, mRootStart);

	if (fi.exist && fi.flag == FLAG_FILE)
	{
		// Create the new file
		int blockId = Create(newFilePath, FLAG_FILE);

		if (blockId == -1)
		{
			// Failed to create file
			return false;
		}
	
		std::string blockContent = readFile(oldFilePath);
		return (WriteFile(blockContent, newFilePath) == 1);
	}

	// Old file didn't exist or wasn't a flag
	return false;
}

std::string FileSystem::listDir(std::string path, int startBlock)
{
	//Cannot List Root until "." and ".." Folders Exist

	if (startBlock == -1)
	{
		startBlock = mRootStart;
	}

	std::string output = "";

	FileInfo fi = Exist(path, startBlock);

	if (fi.exist && fi.flag == FLAG_DIRECTORY) {
		std::string b = mMemblockDevice.readBlock(fi.blockIndex).toString();

		int blockSize = b.size(); //MaxBytes
		const int maxRows = blockSize / rowSize;

		int row = 0;

		bool found = false;
		char a;
		while ((a = b[row*rowSize + INDEX_NodeFlag]) != 0 && row < maxRows)
		{
			std::string nameAtRow = b.substr(row*rowSize + INDEX_NodeFileName, SIZE_NodeFileName);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			int size = 0;
			if (a == FLAG_FILE) {
				size = (unsigned char)(b[row*rowSize + INDEX_NodeFileSize]) << 24 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 1]) << 16 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 2]) << 8 | (unsigned char)(b[row*rowSize + INDEX_NodeFileSize + 3]);
			}

			output += ((a == FLAG_FILE) ? "<FILE>\t" : "<DIR>\t");//Print if it is a file or directory
			output += nameAtRow;//Print Name
			output += ((a == FLAG_FILE) ? "\t" + std::to_string(size) + " Bytes" : "");//If it is a file, Print File Size
			output += "\n";

			row++;
		}
	}
	else {
		output = "Directory does not exist\n";
	}



	return output;
}

void FileSystem::FormatDisk()
{
	mMemblockDevice.reset();
	init();
}

int FileSystem::freeSpace()
{
	return mMemblockDevice.spaceLeft() * BLOCK_SIZE_DEFAULT; //Hardcoded blocksize TODO: FIX
}
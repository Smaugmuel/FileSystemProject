#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileInfo FileSystem::Exist(std::string path, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo f = {false, "" ,FLAG_FILE, -1, -1};

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
		while (name.size() > elementNameSize)
		{
			name.pop_back();
		}

		folders.push_back(name);
	}

	//===============================================================================
	// Read from the root block
	int block = startBlock;

	std::string b = mMemblockDevice.readBlock(block).toString();
	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;

	//If file is not created in current directory
	for (int i = 0; i < folders.size(); i++)
	{
		int row = 0;
		bool found = false;
		char a;
		while ((a = b[row*rowSize]) != 0 && row < maxRows && !found)
		{
			std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			nameAtRow.size();

			if (nameAtRow.compare(folders.at(i)) == 0) {//Folder Exist!

				if (i != folders.size() - 1) {
					if (b[row*rowSize] != FLAG_DIRECTORY) {//Something exist with correct name but it is not a directory. Process can not continue!

						return f;
					}
					else {
						block = b[row*rowSize + 1];//Switch Block nr to the next Folder in pathname.
						found = true;
					}
				}
				else {
					f.exist = true;
					f.fileName = folders.at(i);
					f.flag = b[row*rowSize];
					f.blockIndex = b[row*rowSize + 1];
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

FileSystem::FileSystem() {
	mRootStart = mMemblockDevice.reservBlock();//TODO:: ERROR hadeling if reservBlock() returns -1

	std::string b = mMemblockDevice.readBlock(mRootStart).toString();

	//Add folder "." pointing to itself
	b[0] = FLAG_DIRECTORY;
	b[1] = mRootStart;//
	b.replace(b.begin() + (0*rowSize + nodeInfo), b.begin() + (0*rowSize + nodeInfo + 1), ".");

	//Add folder ".." pointing to parrent(itself becuse root)
	b[1*rowSize] = FLAG_DIRECTORY;
	b[1*rowSize+1] = mRootStart;//
	b.replace(b.begin() + (1*rowSize + nodeInfo), b.begin() + (1*rowSize + nodeInfo + 2), "..");

	//Rewrite root block
	mMemblockDevice.writeBlock(mRootStart,b);
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
		while (name.size() > elementNameSize)
		{
			name.pop_back();
		}

		folders.push_back(name);
	}
	
	// The last name is the file name (currently stored in 'name'), not a folder
	folders.pop_back();

	//===============================================================================
	int block;
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
	while ((a = b[row*rowSize]) != 0 && row < maxRows && !found)
	{
		std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
		nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

		//nameAtRow.size();

		if (nameAtRow.compare(name) == 0) {//File Exist!
			block = b[row*rowSize + 1];
			found = true;
		}

		row++;
	}

	//===============================================================================
	if(found){//File Exist And Cannot be created
		return -1;
	}
	else {

		// f: Flag for a file
		// 1: Which block it's located in
		std::string data = b;

		int blockIndex = mMemblockDevice.reservBlock();
		if (blockIndex == -1) //-1 if no free block could be reserved for the file/folder
			return false;

		data[row*rowSize] = flag;
		data[row*rowSize + 1] = blockIndex;
		data.replace(data.begin() + (row*rowSize + nodeInfo), data.begin() + (row*rowSize + nodeInfo + name.size()), name);

		mMemblockDevice.writeBlock(block, data);

		//===========================
		//Reset and Config new data block
		b = mMemblockDevice.readBlock(blockIndex).toString();
		b.replace(b.begin(), b.end(), blockSize, '\0');
		if (flag == FLAG_DIRECTORY) {
			//Add folder "." pointing to itself
			b[0] = FLAG_DIRECTORY;
			b[1] = blockIndex;//
			b.replace(b.begin() + (0*rowSize + nodeInfo), b.begin() + (0*rowSize + nodeInfo + 1), ".");

			//Add folder ".." pointing to parrent
			b[1*rowSize] = FLAG_DIRECTORY;
			b[1*rowSize+1] = block;//
			b.replace(b.begin() + (1*rowSize + nodeInfo), b.begin() + (1*rowSize + nodeInfo + 2), "..");
		}

		//Rewrite new block
		mMemblockDevice.writeBlock(blockIndex,b);


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
		while ((a = b[row*rowSize]) != 0 && row < maxRows)
		{
			std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			if (nameAtRow.compare(fi.fileName) == 0) {//File Exist!
				rowToRemove = row;
			}

			row++;
		}

		//Replace the "file to be removed"'s row in parrent to the last row in parrent
		b.replace(b.begin() + (rowToRemove*rowSize), b.begin() + (rowToRemove*rowSize + rowSize), b.substr((row-1)*rowSize, rowSize));
		//Clear last Row in Parrent
		b.replace(b.begin() + ((row-1)*rowSize), b.begin() + ((row-1)*rowSize + rowSize),rowSize, '\0');

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

		while ((a = folderItems[row*rowSize]) != 0 && row < maxRows)
		{
			std::string nameAtRow = folderItems.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
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
			while ((a = b[row*rowSize]) != 0 && row < maxRows)
			{
				std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
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

std::string FileSystem::readFile(std::string path, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo fi = Exist(path, startBlock);

	std::string output = "";
	if (fi.exist /*&& fi.flag == FLAG_FILE*/) {
		output = mMemblockDevice.readBlock(fi.blockIndex).toString();
	}

	return output;
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
		while ((a = b[row*rowSize]) != 0 && row < maxRows)
		{
			std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
			nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

			output += ((a == 'f') ? "<FILE>\t" : "<DIR>\t");
			output += nameAtRow;
			output += "\n";

			row++;
		}
	}
	else {
		output = "Directory does not exist\n";
	}



	return output;
}

std:: string FileSystem::GetStringContainingAllBlocks()
{
	std::ostringstream oss;
	std::string currentBlockData;

	for (int i = 0; i < mMemblockDevice.size(); i++)
	{
		currentBlockData = mMemblockDevice[i].toString();
		oss << currentBlockData;
	}

	return oss.str();
}

void FileSystem::RestoreImage(int i, char* data)
{
	mMemblockDevice[i].writeBlock(data);
}

int FileSystem::freeSpace()
{
	return mMemblockDevice.spaceLeft()*512; //Hardcoded blocksize TODO: FIX
}

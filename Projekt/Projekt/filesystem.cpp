#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileInfo FileSystem::Exist(std::string path, int startBlock)
{
	if (startBlock == -1)
		startBlock = mRootStart;

	FileInfo f = {false, 'f', -1};

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
		while (a = b[row*rowSize] != 0 && row < maxRows && !found)
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
					f.flag = b[row*rowSize];
					f.blockIndex = b[row*rowSize + 1];
					
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
	while (a = b[row*rowSize] != 0 && row < maxRows && !found)
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

		return blockIndex;//Return block index of created file/folder
	}

}
#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileSystem::FileSystem() {
	mBlockCount = 0;
}

FileSystem::~FileSystem() {

}

bool FileSystem::createFile(std::string fileName)
{
	// Replace spaces with underlines and slashes with spaces
	// to utilize stringstream easily
	std::replace(fileName.begin(), fileName.end(), ' ', '_');
	std::replace(fileName.begin(), fileName.end(), '/', ' ');

	// Rows consist of 2 bytes for information and 14 for the name
	const int elementNameSize = 13;
	const int nodeInfo = 3;
	const int rowSize = nodeInfo + elementNameSize;

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
	// Read from the root block
	int block = 0;
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

				if (b[row*rowSize] != 'd') {//Something exist with correct name but it is not a directory. Process can not continnue!
					return false;
				}

				block = b[row*rowSize + 1];//Switch Block nr to the next Folder in pathname.
				found = true;
			}

			row++;
		}

		if (found) {//Folder Exist And The Process can continue
			b = mMemblockDevice.readBlock(block).toString();//Read Data from next block
		}
		else {//Folder Does'nt Exist and needs to be created before creating the file inside it!!!!!!!!!!!!!!!!!!!!!
			return false;
		}
	}

	//===============================================================================
	//Create File in current directory
	//Loop Through all files in current block directory(block)
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
		return false;
	}
	else {

		// f: Flag for a file
		// 1: Which block it's located in
		std::string data = b;

		data[row*rowSize] = 'f';
		data[row*rowSize + 1] = ++mBlockCount;
		data.replace(data.begin() + (row*rowSize + nodeInfo), data.begin() + (row*rowSize + nodeInfo + name.size()), name);

		mMemblockDevice.writeBlock(block, data);
	}


}

bool FileSystem::createFolder(std::string folderName)
{
	// Replace spaces with underlines and slashes with spaces
	// to utilize stringstream easily
	std::replace(folderName.begin(), folderName.end(), ' ', '_');
	std::replace(folderName.begin(), folderName.end(), '/', ' ');

	// Rows consist of 2 bytes for information and 14 for the name
	const int elementNameSize = 13;
	const int nodeInfo = 3;
	const int rowSize = nodeInfo + elementNameSize;

	//===============================================================================
	// Create an array of the path's folder names
	std::vector<std::string> folders;
	std::string name;
	std::stringstream ss(folderName);
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
	// Read from the root block
	int block = 0;
	std::string b = mMemblockDevice.readBlock(block).toString();
	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;


	//If Folders is not created in current directory
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

				if (b[row*rowSize] != 'd') {//Something exist with correct name but it is not a directory. Process can not continnue!
					return false;
				}

				block = b[row*rowSize + 1];//Switch Block nr to the next Folder in pathname.
				found = true;
			}

			row++;
		}

		if (found) {//Folder Exist And The Process can continue
			b = mMemblockDevice.readBlock(block).toString();//Read Data from next block
		}
		else {//Folder Does'nt Exist and needs to be created before creating the file inside it!!!!!!!!!!!!!!!!!!!!!
			return false;
		}
	}


	//===============================================================================
	//Create File in current directory
	//Loop Through all files in current block directory(block)
	int row = 0;
	bool found = false;
	char a;
	while (a = b[row*rowSize] != 0 && row < maxRows && !found)
	{
		std::string nameAtRow = b.substr(row*rowSize + nodeInfo, elementNameSize);//file/Foldername found in block
		nameAtRow = nameAtRow.substr(0, nameAtRow.find('\0'));

		nameAtRow.size();

		if (nameAtRow.compare(name) == 0) {//Folder Exist!
			block = b[row*rowSize + 1];
			found = true;
		}

		row++;
	}

	//===============================================================================
	if (found) {//File Exist And Cannot be created
		return false;
	}
	else {

		// f: Flag for a file
		// 1: Which block it's located in
		std::string data = b;

		data.size();

		data[row*rowSize] = 'd';
		data[row*rowSize + 1] = ++mBlockCount;
		data.replace(data.begin() + (row*rowSize + nodeInfo), data.begin() + (row*rowSize + nodeInfo + name.size()), name);

		data.size();

		mMemblockDevice.writeBlock(block, data);
	}
}


/* Please insert your code */
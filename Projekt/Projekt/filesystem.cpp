#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileSystem::FileSystem() {

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
	const int elementNameSize = 14;
	const int nodeInfo = 2;
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
	Block b = mMemblockDevice.readBlock(block);
	int blockSize = b.size(); //MaxBytes
	const int maxRows = blockSize / rowSize;


	//If file is not created in current directory
	//for (int i = 0; i < folders.size()-1; i++)
	//{
	//	
	//	int row = 0;
	//	bool found = false;
	//
	//	//Loop Through all files in current block directory(block)
	//	while (b[row*rowSize] != 0 && row < maxRows && !found)
	//	{
	//		std::string folderFound = b.toString(row*rowSize+nodeInfo, row*rowSize+rowSize);//file/Foldername found in block
	//
	//		if (folderFound == folders.at(i)) {//Folder Exist!
	//			block = b[row*rowSize];
	//			found = true;
	//		}
	//		else {//Folder Does'nt Exist
	//
	//		}
	//
	//		row++;
	//	}
	//}

	//===============================================================================
	//Create File in current directory
	//Loop Through all files in current block directory(block)
	int row = 0;
	bool found = false;
	while (b[row*rowSize] != 0 && row < maxRows && !found)
	{
		std::string folderFound = b.toString(row*rowSize + nodeInfo, row*rowSize + rowSize);//file/Foldername found in block

		if (folderFound == folders.at(folders.size()-1)) {//Folder Exist!
			block = b[row*rowSize];
			found = true;
		}
		else {//Folder Does'nt Exist

		}

		row++;
	}

	//===============================================================================
	if(found){//File Exist And Cannot be created
		return false;
	}
	else {

		// 2: Flag for a file
		// 1: Which block it's located in
		std::string data("21");
		data += name;


		mMemblockDevice.writeBlock(0, data.c_str());
	}


}

void FileSystem::createFolder(std::string folderName)
{
	std::replace(folderName.begin(), folderName.end(), ' ', '_');
	std::replace(folderName.begin(), folderName.end(), '/', ' ');

	std::vector<std::string> folders;
	std::string temp;
	std::stringstream ss(folderName);
	while (ss >> temp)
	{
		folders.push_back(temp);
	}
}


/* Please insert your code */
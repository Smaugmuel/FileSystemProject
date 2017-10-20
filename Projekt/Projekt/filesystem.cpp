#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileSystem::FileSystem() {

}

FileSystem::~FileSystem() {

}

void FileSystem::createFile(std::string fileName)
{
	std::replace(fileName.begin(), fileName.end(), ' ', '_');
	std::replace(fileName.begin(), fileName.end(), '/', ' ');

	std::vector<std::string> folders;
	std::string temp;
	std::stringstream ss(fileName);
	while (ss >> temp)
	{
		folders.push_back(temp);
	}

	int block = 0;

	//
	for (int i = 0; i < folders.size()-1; i++)
	{
		Block b = mMemblockDevice.readBlock(block);
		int blockSize = b.size(); //MaxBytes

		const int elementNameSize = 14;
		const int nodeInfo = 2;
		const int rowSize = nodeInfo + elementNameSize;
		const int maxRows = blockSize / (rowSize);

		int row = 0;

		while (b[row*rowSize] != 0 && row < maxRows)
		{
			std::string folderFound = b.toString(row*rowSize+nodeInfo, row*rowSize+rowSize);

			if (folderFound == folders.at(i)) {//Folder Exist!
			
			}
			else {//Folder Does'nt Exist


			}
		}
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
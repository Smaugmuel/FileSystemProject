#include "filesystem.h"
#include <algorithm>
#include <sstream>

FileSystem::FileSystem() {

}

FileSystem::~FileSystem() {

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
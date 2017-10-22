#include <iostream>
#include <sstream>
#include "filesystem.h"
#include <algorithm>
#include <map>

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 16;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","help", "spaceleft",
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */
void CreateFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void Catenate(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void ListDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void CopyFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void MakeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void PrintWorkingDirectory(const std::string& directory);
void MoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void RemoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void AppendFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void ChangeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void FormatDisk(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void SpaceLeft(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);

std::string TrimPath(std::string path);
std::string ProcessPath(std::string cd, std::string extraPath);

int main(void) {

	std::string userCommand, commandArr[MAXCOMMANDS];
	std::map<std::string, int> users;
	std::string user = "";    // Change this if you want another user to be displayed
	int UserID;

	std::string currentDir = "/";    // current directory, used for output
	int currentDirectoryBlock;

    bool bRun = true;

	FileSystem fs;	

	//Init Users
	//===========================
	users["User1"] = 1;
	users["User2"] = 2;
	users["User3"] = 3;
	users["User4"] = 4;

	//File Test
	//==============================================================================
	
#pragma region Tests
	
	int result;
	
	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl;

	result = fs.Create("/stuff.txt",FLAG_FILE);
	if (result == -1) {
		std::cout << "Failed To Create File(1)" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit", FLAG_DIRECTORY);
	if (result == -1) {
		std::cout << "Failed To Create File(2)" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/th.a", FLAG_FILE);
	if (result == -1) {
		std::cout << "Failed To Create File(3)" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit/stuff.txt", FLAG_FILE);
	if (result == -1) {
		std::cout << "Failed To Create File(4)" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}

	result = fs.Create("/shit2/Hej/Test/stuff.txt", FLAG_FILE); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File(5)" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit2/Hej/Test/Bajs.txt", FLAG_FILE); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit2/Hej/Test/stuff2.txt", FLAG_FILE); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit2/Hej/Test/Kissen.txt", FLAG_FILE); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}

	//Will Try To Remove shit2 folder later with all files in it
	fs.Create("/shit2/Hej/Test/Kissen2.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/Kissen3.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/Kissen4.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/Kissen5.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/MerMappDjup/Kissen1.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/MerMappDjup/Kissen2.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/MerMappDjup/Kissen3.txt", FLAG_FILE);
	fs.Create("/shit2/Hej/Test/MerMappDjup/Kissen4.txt", FLAG_FILE);

	result = fs.Create("/shit2/Hej/Test/Mapp1", FLAG_DIRECTORY); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}
	result = fs.Create("/shit2/Hej/Test/Mapp2", FLAG_DIRECTORY); //Many Directorys Created At The Same Time!
	if (result == -1) {
		std::cout << "Failed To Create File" << std::endl;
	}
	else {
		std::cout << "File created at Block: " << result << std::endl;
	}

	//REMOVE TEST
	//====================================

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace()/BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl;

	bool res = fs.Remove("/stuff.txt");//Remove File (Stuff.txt)
	if (!res) {
		std::cout << "Failed To Remove File(1)" << std::endl;
	}
	else {
		std::cout << "File Removed(1)" << std::endl;
	}

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl;

	res = fs.Remove("/shit2");//Remove Folder With many sub fils & folders
	if (!res) {
		std::cout << "Failed To Remove File(1)" << std::endl;
	}
	else {
		std::cout << "File Removed(1)" << std::endl;
	}

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl;

	//LIST FILES IN DIRECTORY
	//================================
	std::cout << std::endl << fs.listDir("./") << std::endl;

	//==============================================================================

#pragma endregion 


	// NOTE
	//----------------------
	// Currently, commands aren't looped when multiple are inserted
	// ls stuff   <=>   ls stuff stuff2

    do {
        std::cout << user << ":" << currentDir << "$ ";
        getline(std::cin, userCommand);

        int nrOfCommands = parseCommandString(userCommand, commandArr);
        if (nrOfCommands > 0) {

            int cIndex = findCommand(commandArr[0]);
            switch(cIndex) {

			case 0: //quit
				bRun = quit();                
                break;
            case 1: // format
				FormatDisk(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 2: // ls
				ListDirectory(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 3: // create
				CreateFile(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 4: // cat
				Catenate(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 5: // createImage
                break;
            case 6: // restoreImage
                break;
            case 7: // rm
				RemoveFile(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 8: // cp
				CopyFile(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 9: // append
				AppendFile(nrOfCommands,fs,commandArr,currentDir);
                break;
            case 10: // mv
				MoveFile(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 11: // mkdir
				MakeDirectory(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 12: // cd
				ChangeDirectory(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 13: // pwd
				PrintWorkingDirectory(currentDir);
                break;
            case 14: // help
                std::cout << help() << std::endl;
                break;
			case 15: // spaceleft
				SpaceLeft(nrOfCommands, fs, commandArr, currentDir);
				break;

            default:
                std::cout << "Unknown command: " << commandArr[0] << std::endl;
            }
        }
    } while (bRun == true);

    return 0;
}

int parseCommandString(const std::string &userCommand, std::string strArr[]) {
    std::stringstream ssin(userCommand);
    int counter = 0;
    while (ssin.good() && counter < MAXCOMMANDS) {
        ssin >> strArr[counter];
        counter++;
    }
    if (strArr[0] == "") {
        counter = 0;
    }
    return counter;
}
int findCommand(std::string &command) {
    int index = -1;
    for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
        if (command == availableCommands[i]) {
            index = i;
        }
    }
    return index;
}

bool quit() {
	std::cout << "Exiting\n";
	return false;
}

std::string help() {
    std::string helpStr;
    helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
    helpStr += "-----------------------------------------------------------------------------------\n" ;
    helpStr += "* quit:                             Quit OSD Disk Tool\n";
    helpStr += "* format;                           Formats disk\n";
    helpStr += "* ls     <path>:                    Lists contents of <path>.\n";
    helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
    helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
    helpStr += "* createImage  <real-file>:         Saves disk to <real-file>\n";
    helpStr += "* restoreImage <real-file>:         Reads <real-file> onto disk\n";
    helpStr += "* rm     <file>:                    Removes <file>\n";
    helpStr += "* cp     <source> <destination>:    Copy <source> to <destination>\n";
    helpStr += "* append <source> <destination>:    Appends contents of <source> to <destination>\n";
    helpStr += "* mv     <old-file> <new-file>:     Renames <old-file> to <new-file>\n";
    helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
    helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
    helpStr += "* pwd:                              Get current working directory\n";
    helpStr += "* help:                             Prints this help screen\n";
    return helpStr;
}

/* Insert code for your shell functions and call them from the switch-case */

void CreateFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	if (nrOfCommands > 1) {
		if (fs.Create(ProcessPath(currentDir, commandArr[1]), FLAG_FILE) != -1) {
			std::cout << "Created File: " << commandArr[1] << "\nInsert content: ";

			std::string content;
			getline(std::cin, content);

			switch (fs.WriteFile(content, ProcessPath(currentDir, commandArr[1])))
			{
			case 1:
				std::cout << "Wrote to file\n\n";
				break;
			case -1:
				std::cout << "Couldn't write to file: Out of range\n\n";
				break;
			case -2:
				std::cout << "Couldn't write to file: Wrong size\n\n";
				break;
			case -3:
				std::cout << "Couldn't write to file: Content couldn't fit\n\n";
				break;
			case -4:
				std::cout << "Couldn't write to file: File does not exist\n\n";
				break;
			default:
				break;
			}
		}
		else {
			std::cout << "Could Not Create file: " << commandArr[1] << "\n\n";
		}
	}
	else {
		std::cout << "Wrong Syntax! Insert Path to Create file\n\n";
	}
}

void Catenate(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	if (nrOfCommands > 1) {
		std::cout << fs.readFile(ProcessPath(currentDir, commandArr[1])) << "\n\n";
	}
	else {
		std::cout << "Wrong Syntax! Insert Path to Read from\n\n";
	}
}

void ListDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	std::cout << "Listing directory\n";
	std::cout << fs.listDir(currentDir + (nrOfCommands > 1 ? ProcessPath(currentDir, commandArr[1]) : ".")) << "\n";
}

void CopyFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	if (nrOfCommands > 2)
	{
		if (fs.CopyFile(ProcessPath(currentDir, commandArr[1]), ProcessPath(currentDir, commandArr[2])))
		{
			std::cout << "Created file " << commandArr[2] << " as a copy of " << commandArr[1] << "\n\n";
		}
		else
		{
			std::cout << "Failed to copy file " << commandArr[1] << "\n\n";
		}
	}
	else
	{
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
}

void MakeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	if (nrOfCommands > 1)
	{
		if (fs.Create(ProcessPath(currentDir, commandArr[1]), FLAG_DIRECTORY) != -1)
		{
			std::cout << "Created directory " << commandArr[1] << "\n\n";
		}
		else
		{
			std::cout << "Failed to create directory " << commandArr[1] << "\n\n";
		}
	}
	else
	{
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
}

void PrintWorkingDirectory(const std::string& directory)
{
	std::cout << directory << "\n";
}

void MoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	if (nrOfCommands > 2)
	{
		if (fs.MoveFile(currentDir + commandArr[1], currentDir + commandArr[2]))
		{
			std::cout << "Moved file " << commandArr[1] << "\n\n";
		}
		else
		{
			std::cout << "Fel\n\n";
		}
		std::cout << "Fel\n\n";
	}
	std::cout << "Fel\n\n";
}

void RemoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {
	if (nrOfCommands > 1) {
		for (int i = 1; i < nrOfCommands; i++)
		{
			bool res = fs.Remove(ProcessPath(currentDir, commandArr[1]));//Remove File (Stuff.txt)
			if (!res) {
				std::cout << "Failed To Remove: " << commandArr[i] << std::endl;
			}
			else {
				std::cout << "Removed: " << commandArr[i] << std::endl;
			}
		}
	}
	else {
		std::cout << "Wrong sntax! type help for help" << std::endl;
	}
}

void AppendFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {
	if (nrOfCommands == 2) {
		std::string content;
		std::cout << "Content to appent with: ";
		getline(std::cin, content);
		
		switch (fs.AppendFile(content, ProcessPath(currentDir, commandArr[1])))
		{
		case 1:
			std::cout << "Wrote to file\n\n";
			break;
		case -1:
			std::cout << "Couldn't write to file: Out of range\n\n";
			break;
		case -2:
			std::cout << "Couldn't write to file: Wrong size\n\n";
			break;
		case -3:
			std::cout << "Couldn't write to file: Content couldn't fit\n\n";
			break;
		case -4:
			std::cout << "Couldn't write to file: File does not exist\n\n";
			break;
		default:
			break;
		}

	}
	else if (nrOfCommands == 3) {
		std::string data = fs.readFile(ProcessPath(currentDir, commandArr[1]));

		switch (fs.AppendFile(data, ProcessPath(currentDir, commandArr[2])))
		{
		case 1:
			std::cout << "Wrote to file\n\n";
			break;
		case -1:
			std::cout << "Couldn't write to file: Out of range\n\n";
			break;
		case -2:
			std::cout << "Couldn't write to file: Wrong size\n\n";
			break;
		case -3:
			std::cout << "Couldn't write to file: Content couldn't fit\n\n";
			break;
		case -4:
			std::cout << "Couldn't write to file: File does not exist\n\n";
			break;
		default:
			break;
		}
	}
	else {
		std::cout << "Wrong Syntax! Insert Path to Create file\n\n";
	}
}

void ChangeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {
	
	FileInfo fi = fs.Exist(ProcessPath(currentDir, commandArr[1]));

	if (fi.exist && fi.flag == FLAG_DIRECTORY) {
		currentDir += commandArr[1];
		currentDir = TrimPath(currentDir) + "/";
	}
	else
	{
		std::cout << "Path entered is not a directory\n\n";
	}
}

void FormatDisk(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {

	std::string content;

	bool erase = false;
	bool stop = false;

	do {
		std::cout << "Are you sure you want to format the disk? all data will be erased ( yes / no ): ";
		getline(std::cin, content);
		if (content == "yes") {
			erase = true;
			stop = true;
		}
		else if(content == "no")
		{
			erase = false;
			stop = true;
		}
			

	} while (!stop);

	if (erase) {
		currentDir = "/";
		fs.FormatDisk();

		std::cout << "\nDisk is formated. All data have been removed" << std::endl;
		std::cout << "Space Left On Disk: " << fs.freeSpace() << " Bytes (" << fs.freeSpace() / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl << std::endl;
	}
}

void SpaceLeft(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {
	std::cout << "Space Left On Disk: "<< fs.freeSpace() << " Bytes (" << fs.freeSpace() / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl << std::endl;
}


std::string ProcessPath(std::string cd, std::string extraPath) {

	while (extraPath[extraPath.size()-1] == '/')
	{
		extraPath.pop_back();
	}

	std::string start = extraPath.substr(0,2);
	if (start == "/.") {
		return extraPath;
	}
	else {
		return cd + extraPath;
	}
	
}

std::string TrimPath(std::string path) {
	// Replace spaces with underlines and slashes with spaces
	// to utilize stringstream easily
	std::replace(path.begin(), path.end(), ' ', '_');
	std::replace(path.begin(), path.end(), '/', ' ');
	
	//===============================================================================
	// Create an array of the path's folder names
	std::vector<std::string> folders;
	std::vector<std::string> folders2;
	std::string name;
	std::stringstream ss(path);
	while (ss >> name)
	{
		folders.push_back(name);
	}
	for (int i = 0; i < folders.size(); i++)
	{
		if (folders.at(i) == "..") {
			folders2.pop_back();
		}
		else if (folders.at(i) != ".") {
			folders2.push_back(folders.at(i));
		}
	}

	std::string output;
	output += "/";

	for (int i = 0; i < folders2.size(); i++)
	{
		output += folders2.at(i) + "/";
	}

	output.pop_back();

	return output;
}
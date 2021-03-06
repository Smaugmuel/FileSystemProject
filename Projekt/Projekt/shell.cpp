#include <iostream>
#include <sstream>
#include <fstream>
#include "filesystem.h"
#include <algorithm>
#include <map>

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 18;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","spaceleft","cu","chmod","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);

bool quit();
void FormatDisk(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void ListDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void CreateFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void Catenate(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);


void RemoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void CopyFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void AppendFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void MoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void MakeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
void ChangeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void PrintWorkingDirectory(const std::string& directory);
void SpaceLeft(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir);
void ChangeUser(unsigned int nrOfCommands, std::string commandArr[], std::map<std::string, int>& users, std::string& currentUser, int& currentUserID);
void Chmod(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID);
std::string help();
void CreateImage(FileSystem& fs, std::string name);
void ReadImage(FileSystem& fs, std::string name);

std::string TrimPath(std::string path);
std::string ProcessPath(std::string cd, std::string extraPath);

int main(void)
{
	std::string userCommand, commandArr[MAXCOMMANDS];
	std::map<std::string, int> users;
	std::string currentUser = "User1";    // Change this if you want another user to be displayed
	int UserID = 1;

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

    do {
        std::cout << currentUser << ":" << currentDir << "$ ";
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
				CreateFile(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 4: // cat
				Catenate(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 5: // createImage
				if (nrOfCommands > 1) {
					CreateImage(fs, commandArr[1]);
					std::cout << "Restored filesystem to: " << commandArr[1] << std::endl;
				}
				else {
					std::cout << "Wrong Syntax! Give the saved file a name!" << std::endl;
				}				
                break;
            case 6: // restoreImage
				if (nrOfCommands > 1) {
					ReadImage(fs, commandArr[1]);
				}
				else {
					std::cout << "Wrong Syntax! Give the restored file a name!" << std::endl;
				}
                break;
            case 7: // rm
				RemoveFile(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 8: // cp
				CopyFile(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 9: // append
				AppendFile(nrOfCommands,fs,commandArr,currentDir, UserID);
                break;
            case 10: // mv
				MoveFile(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 11: // mkdir
				MakeDirectory(nrOfCommands, fs, commandArr, currentDir, UserID);
                break;
            case 12: // cd
				ChangeDirectory(nrOfCommands, fs, commandArr, currentDir);
                break;
            case 13: // pwd
				PrintWorkingDirectory(currentDir);
                break;
			case 14: // spaceleft
				SpaceLeft(nrOfCommands, fs, commandArr, currentDir);
				break;
			case 15: // cu
				ChangeUser(nrOfCommands, commandArr, users, currentUser, UserID);
				break;
			case 16: // chmod
				Chmod(nrOfCommands, fs, commandArr, currentDir, UserID);
				break;
            case 17: // help
                std::cout << help() << std::endl;
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
    helpStr += "* format:                           Formats disk\n";
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
	helpStr += "* spaceleft [<-Extras>]:            Shows how much space is left in file system. (Extra: <-nt> print no extra text. <-Kb> print size in Kb. <-Mb> print size in Mb.  <-Gb> print size in Gb. )\n";
	helpStr += "* cu     <username>:                Changes user to <username>\n";
	helpStr += "* chmod  <access right> <filename>: Changes access rights of each user except owner of <filename> to <access right> (r- : Read only, rw : Read and write, -w : Write only, -- : No access\n";
    helpStr += "* help:                             Prints this help screen\n";
    return helpStr;
}

void CreateImage(FileSystem& fs, std::string name) {

	std::ofstream txtfile(name);
	
	txtfile << fs.GetStringContainingAllBlocks();

	txtfile.close();

}

void ReadImage(FileSystem& fs, std::string name) {
	std::ifstream File(name);
	char blockData[512];
	int j = 0;

	while (!File.eof() && j < 250)
	{
		File.read(blockData,sizeof(blockData));
		fs.RestoreImage(j,blockData);
		j++;
	}

	File.close();
}

/* Insert code for your shell functions and call them from the switch-case */

void CreateFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID)
{
	if (nrOfCommands > 1) {
		if (fs.Create(UserID, ProcessPath(currentDir, commandArr[1]), FLAG_FILE) != -1) {
			std::cout << "Created File: " << commandArr[1] << "\nInsert content: ";

			std::string content;
			getline(std::cin, content);

			switch (fs.WriteFile(UserID, content, ProcessPath(currentDir, commandArr[1])))
			{
			case 1:
				std::cout << "Wrote to file\n\n";
				break;
			case -1:
				std::cout << "Couldn't write to file: Disk full\n\n";
				break;
			case -2:
				std::cout << "Couldn't write to file: File not Found\n\n";
				break;
			case -5:
				std::cout << "Couldn't write to file: Access Denied\n\n";
				break;
			default:
				std::cout << "Couldn't write to file: Unknown Error\n\n";
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

void Catenate(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID)
{
	if (nrOfCommands > 1) {
		std::cout << fs.readFile(UserID, ProcessPath(currentDir, commandArr[1])) << "\n\n";
	}
	else {
		std::cout << "Wrong Syntax! Insert Path to Read from\n\n";
	}
}

void ListDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir)
{
	std::cout << "Listing directory\n";
	std::cout << fs.listDir((nrOfCommands > 1 ? ProcessPath(currentDir, commandArr[1]) : currentDir + ".")) << "\n";
}

void CopyFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID)
{
	if (nrOfCommands > 2)
	{
		switch (fs.CopyFile(UserID, ProcessPath(currentDir, commandArr[1]), ProcessPath(currentDir, commandArr[2])))
		{
		case 1:
			std::cout << "Copyed file\n\n";
			break;
		case -1:
			std::cout << "Couldn't copy file: Disk full\n\n";
			break;
		case -2:
			std::cout << "Couldn't copy file: New file got corrupted\n\n";
			break;
		case -3:
			std::cout << "Couldn't copy file: File not Found or is Directory\n\n";
			break;
		case -5:
			std::cout << "Couldn't copy file: Access Denied\n\n";
			break;
		default:
			std::cout << "Couldn't copy file: Unknown Error\n\n";
			break;
		}
	}
	else
	{
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
}

void MakeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID)
{
	if (nrOfCommands > 1)
	{
		if (fs.Create(UserID, ProcessPath(currentDir, commandArr[1]), FLAG_DIRECTORY) != -1)
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

void MoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID)
{
	if (nrOfCommands > 2)
	{
		switch (fs.MoveFile(UserID, ProcessPath(currentDir, commandArr[1]), ProcessPath(currentDir, commandArr[2])))
		{
		case 1:
			std::cout << "Successfully moved file " << commandArr[1] << "\n\n";
			break;
		case -1:
			std::cout << "Block number out of range\n\n";
			break;
		case -2:
			std::cout << "Varying block dimensions\n\n";
			break;
		case -3:
			std::cout << "Couldn't move file: Targeted directory does'not exist. Please create it first\n\n";
			break;
		case -4:
			std::cout << "Couldn't move file: File " << commandArr[1] << " was not found\n\n";
			break;
		case -5:
			std::cout << "Couldn't move file: Access Denied\n\n";
			break;
		default:
			break;
		}
	}
	else
	{
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
}

void RemoveFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID) {
	if (nrOfCommands > 1) {
		for (int i = 1; i < nrOfCommands; i++)
		{
			bool res = fs.Remove(UserID, ProcessPath(currentDir, commandArr[1]));//Remove File (Stuff.txt)
			if (!res) {
				std::cout << "Failed To Remove: " << commandArr[i] << std::endl;
			}
			else {
				std::cout << "Removed: " << commandArr[i] << std::endl;
			}
		}
	}
	else {
		std::cout << "Wrong syntax! Type help for help" << std::endl;
	}
}

void AppendFile(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID) {
	if (nrOfCommands == 2) {
		std::string content;
		std::cout << "Content to appent with: ";
		getline(std::cin, content);
		
		switch (fs.AppendFile(UserID, content, ProcessPath(currentDir, commandArr[1])))
		{
		case 1:
			std::cout << "Wrote to file\n\n";
			break;
		case -1:
			std::cout << "Couldn't write to file: Disk full\n\n";
			break;
		case -2:
			std::cout << "Couldn't write to file: File not Found\n\n";
			break;
		case -3:
			std::cout << "Couldn't write to file: Content couldn't fit\n\n";
			break;
		case -4:
			std::cout << "Couldn't write to file: Can't Append Directory\n\n";
			break;
		case -5:
			std::cout << "Couldn't write to file: Access Denied\n\n";
			break;
		default:
			std::cout << "Couldn't write to file: Unkown Error\n\n";
			break;
		}

	}
	else if (nrOfCommands == 3) {
		std::string data = fs.readFile(UserID, ProcessPath(currentDir, commandArr[1]));

		if (data != "") {
			switch (fs.AppendFile(UserID, data, ProcessPath(currentDir, commandArr[2])))
			{
			case 1:
				std::cout << "Wrote to file\n\n";
				break;
			case -1:
				std::cout << "Couldn't write to file: Disk full\n\n";
				break;
			case -2:
				std::cout << "Couldn't write to file: File not Found\n\n";
				break;
			case -3:
				std::cout << "Couldn't write to file: Content couldn't fit\n\n";
				break;
			case -4:
				std::cout << "Couldn't write to file: Can't Append Directory\n\n";
				break;
			case -5:
				std::cout << "Couldn't write to file: Access Denied\n\n";
				break;
			default:
				std::cout << "Couldn't write to file: Unkown Error\n\n";
				break;
			}
		}

	}
	else {
		std::cout << "Wrong Syntax! Insert Path to Create file\n\n";
	}
}

void ChangeDirectory(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir) {
	std::string newPath = ProcessPath(currentDir, commandArr[1]);
	FileInfo fi = fs.Exist(newPath);

	if (fi.exist && fi.flag == FLAG_DIRECTORY) {
		currentDir = newPath;
		currentDir = TrimPath(newPath) + "/";
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
	
	long Originalsize = fs.freeSpace();

	if (nrOfCommands == 1) {
		std::cout << "Space Left On Disk: " << Originalsize << " Bytes (" << Originalsize / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl << std::endl;
	}
	else {
		bool noText = false;
		int lvl = 0;
		long newSize;
		std::string unit = "Bytes";

		for (int i = 1; i < nrOfCommands; i++)
		{
			if (commandArr[i] == "-nt") {
				noText = true;
			}
			else if (commandArr[i] == "-Kb") {
				lvl = 10;
				unit = "Kb";
			}
			else if (commandArr[i] == "-Mb") {
				lvl = 20;
				unit = "Mb";
			}
			else if (commandArr[i] == "-Gb") {
				lvl = 30;
				unit = "Gb";
			}
		}
		
		newSize = Originalsize >> lvl;//Divide size to
		if (noText) {
			std::cout << newSize << std::endl;
		}
		else {
			std::cout << "Space Left On Disk: " << newSize << " " << unit << " (" << Originalsize / BLOCK_SIZE_DEFAULT << " Blocks)" << std::endl << std::endl;
		}

		
	}
}

void ChangeUser(unsigned int nrOfCommands, std::string commandArr[], std::map<std::string, int>& users, std::string& currentUser, int& currentUserID)
{
	if (nrOfCommands > 1)
	{
		if (users.find(commandArr[1]) != users.end())
		{
			currentUser = commandArr[1];
			currentUserID = users[currentUser];

			std::cout << "Changed user to " << currentUser << "\n\n";
		}
		else
		{
			std::cout << "User " << commandArr[1] << " not found\n\n";
		}
	}
	else
	{
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
}

void Chmod(unsigned int nrOfCommands, FileSystem& fs, std::string commandArr[], std::string& currentDir, int UserID) {
	if (nrOfCommands == 3) {
		if (commandArr[1].size() == 2) {

			char c = 12;
			if (commandArr[1][0] == 'R' || commandArr[1][0] == 'r') {
				c += 1;
			}
			if (commandArr[1][1] == 'W' || commandArr[1][1] == 'w') {
				c += 2;
			}
			
			fs.Cmod(UserID, ProcessPath(currentDir, commandArr[2]), c, UserID);
		}
		else
		{
			std::cout << "Wrong syntax! Type help for correct syntax\n\n";
		}
	}
	else {
		std::cout << "Wrong syntax! Type help for correct syntax\n\n";
	}
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
		if (folders.at(i) == ".." && !folders2.empty()) {
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
		if(folders2.at(i) != "..")
			output += folders2.at(i) + "/";
	}

	output.pop_back();

	return output;
}
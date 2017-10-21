#include <iostream>
#include <sstream>
#include "filesystem.h"

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 15;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */


int main(void) {

	std::string userCommand, commandArr[MAXCOMMANDS];
	std::string user = "user@DV1492";    // Change this if you want another user to be displayed
	std::string currentDir = "/";    // current directory, used for output
	int currentDirectoryBlock;

    bool bRun = true;


	FileSystem fs;
	//fs.createFolder("/hello world");
	

	//File Test
	//==============================================================================
	int result;
	
	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / 512 << " Blocks)" << std::endl;

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
	result = fs.Create("/thing.abd", FLAG_FILE);
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

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace()/512 << " Blocks)" << std::endl;

	bool res = fs.Remove("/stuff.txt");//Remove File (Stuff.txt)
	if (!res) {
		std::cout << "Failed To Remove File(1)" << std::endl;
	}
	else {
		std::cout << "File Removed(1)" << std::endl;
	}

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / 512 << " Blocks)" << std::endl;

	res = fs.Remove("/shit2");//Remove Folder With many sub fils & folders
	if (!res) {
		std::cout << "Failed To Remove File(1)" << std::endl;
	}
	else {
		std::cout << "File Removed(1)" << std::endl;
	}

	std::cout << fs.freeSpace() << " Bytes Free (" << fs.freeSpace() / 512 << " Blocks)" << std::endl;

	//LIST FILES IN DIRECTORY
	//================================
	std::cout << std::endl << fs.listDir("./") << std::endl;

	//==============================================================================



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
                break;
            case 2: // ls
                std::cout << "Listing directory" << std::endl;
				std::cout << fs.listDir(currentDir + (nrOfCommands > 1 ? commandArr[1] : ".")) << std::endl;

                break;
            case 3: // create
				if (nrOfCommands > 1) {
					if (fs.Create(currentDir + commandArr[1], FLAG_FILE) != -1) {
						std::cout << "Created File: " << commandArr[1] << std::endl;
					}
					else {
						std::cout << "Could Not Create file: " << commandArr[1] << std::endl;
					}
				}
				else {
					std::cout << "Wrong Syntax! Insert Path to Create file" << std::endl;
				}
                break;
            case 4: // cat
				if (nrOfCommands > 1) {
					std::cout << fs.readFile(commandArr[1]) << std::endl;
				}
				else {
					std::cout << "Wrong Syntax! Insert Path to Read from" << std::endl;
				}
                break;
            case 5: // createImage
                break;
            case 6: // restoreImage
                break;
            case 7: // rm
                break;
            case 8: // cp
                break;
            case 9: // append
                break;
            case 10: // mv
                break;
            case 11: // mkdir
                break;
            case 12: // cd
                break;
            case 13: // pwd
                break;
            case 14: // help
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

#include "memblockdevice.h"
#include <stdexcept>

void MemBlockDevice::changeBlockStatus(int blockNr, bool used)
{
	//TODO: Error Handling (Out of bounds)

	//Each bit in each byte(on block 0) represents a block on the disk
	//Each Byte have status bit for 8 blocks

	/*
	
	Block 0's structure Explained in image.
	Known "Bug": a maximum of 8 bit * 512 bytes = 4096 free blocks is allowed for now. But we only use 250 blocks so it is fine!

	//F = Free, U = Used

					Byte #1								Byte #2									Byte #3								...	Byte #N
	Exempel	Values	[0] [1] [0] [1] [1] [1] [0] [0]	-	[0] [1] [1] [0] [0] [1] [1] [0]		-	[1] [0] [0] [1] [1] [0] [0] [0]		-	[?]    [?]    [?]    [?]    [?]    [?]    [?]    [?]
	BlockStatus		[F] [U] [F] [U] [U] [U] [F] [F]	-	[F] [U] [U] [F] [F] [U] [U] [F]		-	[U] [F] [F] [U] [U] [F] [F] [F]		-	[?]    [?]    [?]    [?]    [?]    [?]    [?]    [?]
	Block/Bit id	[7] [6] [5] [4] [3] [2] [1] [0]	-	[15][14][13][12][11][10][9] [8]		-	[23][22][21][20][19][18][17][16]	-	[N*8+7][N*8+6][N*8+5][N*8+4][N*8+3][N*8+2][N*8+1][N*8]
	RelativeBitId	[7] [6] [5] [4] [3] [2] [1] [0]	-	[7] [6] [5] [4] [3] [2] [1] [0]		-	[7] [6] [5] [4] [3] [2] [1] [0]		-	[7]    [6]    [5]    [4]    [3]    [2]    [1]    [0]

	*/

	//Finds the right Byte containing status bit for desired bit.
	char statusByte = memBlocks[0][blockNr/8];

	char currentStatus = (statusByte >> (blockNr % 8)) & 1;//Gets the current status of desired block.

	if (used) {//If true, set status bit for the desired block to a 1(in use)
		if (currentStatus == 0) {//Checks if bit not allredy is equal to 1
			statusByte |= used << (blockNr % 8);
			nrOfFreeBlocks--;//decrese nr of free blocks
		}
	}
	else {//If false, set status bit for the desired block to a 0(not in use)
		if (currentStatus == 1) {//Checks if bit not allredy is equal to 0
			statusByte &= ~(1 << (blockNr % 8));
			nrOfFreeBlocks++;//increse nr of free blocks
		}
	}	

	//rewrite byte containing statis bit to block
	memBlocks[0].write(statusByte,blockNr/8);
}

MemBlockDevice::MemBlockDevice(int nrOfBlocks): BlockDevice(nrOfBlocks) {
	changeBlockStatus(0,true);//Set Block containing Block Bit Map as being used
}

MemBlockDevice::MemBlockDevice(const MemBlockDevice &other) : BlockDevice(other) {

}

MemBlockDevice::~MemBlockDevice() {
    /* Implicit call to base-class destructor */
}

MemBlockDevice& MemBlockDevice::operator=(const MemBlockDevice &other) {
    delete [] this->memBlocks;
    this->nrOfBlocks = other.nrOfBlocks;
    this->nrOfFreeBlocks = other.nrOfFreeBlocks;
    this->memBlocks = new Block[this->nrOfBlocks];

    for (int i = 0; i < this->nrOfBlocks; ++i)
        this->memBlocks[i] = other.memBlocks[i];

    return *this;
}

Block& MemBlockDevice::operator[](int index) const {
    if (index < 0 || index >= this->nrOfBlocks) {
        throw std::out_of_range("Illegal access\n");
    }
    else {
        return this->memBlocks[index];
    }
}

int MemBlockDevice::spaceLeft() const {
    return nrOfFreeBlocks;
}

int MemBlockDevice::writeBlock(int blockNr, const std::vector<char> &vec) {
    int output = -1;    // Assume blockNr out-of-range

    if (blockNr < this->nrOfBlocks && blockNr >= 0) {
        /* -2 = vec and block dont have same dimensions */
        /* 1 = success */
        output = this->memBlocks[blockNr].writeBlock(vec);
    }
    return output;
}

int MemBlockDevice::writeBlock(int blockNr, const std::string &strBlock) {
    int output = -1;    // Assume blockNr out-of-range

    if (blockNr < this->nrOfBlocks && blockNr >= 0) {
        /* -2 = str-length and block dont have same dimensions */
        /* 1 = success */
        output = this->memBlocks[blockNr].writeBlock(strBlock);
    }
    return output;
}

int MemBlockDevice::writeBlock(int blockNr, const char cArr[]) {
    int output = -1;    // Assume blockNr out-of-range
    if (blockNr < this->nrOfBlocks && blockNr >= 0) {
        output = 1;
        // Underlying function writeBlock cannot check array-dimension.
        this->memBlocks[blockNr].writeBlock(cArr);
    }
    return output;
}

int MemBlockDevice::reservBlock()
{
	int output = -1;

	if (nrOfFreeBlocks != 0) {
		bool foundFreeBlock = false;
		int currentByteIndex = 0;

		while (!foundFreeBlock)
		{
			//Loop through all bytes in block 0 until a free block is found.
			char currentByte = memBlocks[0][currentByteIndex];

			//Loop through all bits in Byte until a free block is found.
			for (int i = 0; i < 8 && !foundFreeBlock; i++)
			{
				char currentStatus = (currentByte >> i) & 1;//Gets the current status of desired block.
				if (currentStatus == 0) {//Free Block Found
					foundFreeBlock = true;
					output = currentByteIndex * 8 + i;//Block Index to free Block
					changeBlockStatus(output,true);//Change status of free block to "Used"
				}
			}
			
			currentByteIndex++;
		}

	}

	return output;
}

void MemBlockDevice::freeBlock(int blockNr)
{
	changeBlockStatus(blockNr, false);//Change status of block to "not Used"
}

Block MemBlockDevice::readBlock(int blockNr) const {
    if (blockNr < 0 || blockNr >= this->nrOfBlocks)
        throw std::out_of_range("Block out of range");
    else {
        Block a(this->memBlocks[blockNr]);
        return a;
    }
}

/* Resets all the blocks */
void MemBlockDevice::reset() {
    for (int i = 0; i < this->nrOfBlocks; ++i) {
        this->memBlocks[i].reset('0');				//TODO 0 != '0'
    }
}

int MemBlockDevice::size() const {
    return this->nrOfBlocks;
}

#ifndef MEMBLOCKDEVICE_H
#define MEMBLOCKDEVICE_H
#include "blockdevice.h"

class MemBlockDevice: public BlockDevice
{
private:
	void changeBlockStatus(int blockNr, bool free);
	void init();

	int nrOfStatusBitsInBlock;
	int nrOfBlocksInBitMap;
public:
    MemBlockDevice(int nrOfBlocks = 250);
    MemBlockDevice(const MemBlockDevice &other);

    ~MemBlockDevice();

    /* Operators */
    MemBlockDevice& operator=(const MemBlockDevice &other);
    Block &operator[] (int index) const;

    /* Returns amount of free blocks */
    int spaceLeft() const;

    /* Writes a block */
    int writeBlock(int blockNr, const std::vector<char> &vec);
    int writeBlock(int blockNr, const std::string &strBlock);
    int writeBlock(int blockNr, const char cArr[]);     // Use with care, make sure that cArr has at least the same dimension as block

	//Reserv a free block to a file or folder and return the block index. This Block will be marked as "In Used" untill it is freed using freeBlock(int index)
	int reservBlock();//Added

	//Free a block, allowing another file/folder to use it, freeing up space on disk
	void freeBlock(int blockNr);//Added

    /* Reads a block */
    Block readBlock(int blockNr) const;

    /* Resets all the blocks */
    void reset();

    /* Return the size */
    int size() const;
};

#endif // MEMBLOCKDEVICE_H

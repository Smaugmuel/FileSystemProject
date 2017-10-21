#include "blockdevice.h"

BlockDevice::BlockDevice(int nrOfBlocks) {
    if (nrOfBlocks > 0)
        this->nrOfBlocks = nrOfBlocks;
    else
        this->nrOfBlocks = 250;

    this->memBlocks = new Block[this->nrOfBlocks];
    this->nrOfFreeBlocks = nrOfBlocks;

}

BlockDevice::BlockDevice(const BlockDevice &other) {
    this->nrOfBlocks = other.nrOfBlocks;
    this->nrOfFreeBlocks = other.nrOfFreeBlocks;
    this->memBlocks = new Block[this->nrOfBlocks];

    for (int i = 0; i < this->nrOfBlocks; ++i)
        this->memBlocks[i] = other.memBlocks[i];
}

BlockDevice::~BlockDevice() {
    delete [] this->memBlocks;
}



/**
 * libdiskimage
 * VMDK Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VMDK block storage
 */

#ifndef DIVMDKBLOCKSTORAGE_H
#define DIVMDKBLOCKSTORAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIBlockStorage.h"

class DIVMDKBlockStorage : public DIBlockStorage
{
public:
    DIVMDKBlockStorage();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    bool isWriteEnabled();
    DIInt getBlockNum();
    string getFormatLabel();
    
    DIInt getCylinders();
    DIInt getHeads();
    DIInt getSectors();
    
    bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore *backingStore;
    
    DIInt blockNum;
    
    DIInt cylinders;
    DIInt heads;
    DIInt sectors;
    
    DIInt grainSize;
    DIInt grainTableSize;
    DIInt directory1Block;
    DIInt directory2Block;
    bool redundantDirectory;
    DIInt allocatedBlockNum;
    
    vector<DIInt> metadata;
    
    DIInt directoryEntrySize;
    
    bool parseDescriptor(DIBackingStore *backingStore,
                         DILong offset, DIInt size);
    bool setInUse(bool value);
    bool isBlockEmpty(const DIChar *buf);
    DIInt allocateGrain(DIInt directoryIndex, DIInt grainTableIndex);
};

#endif


/**
 * libdiskimage
 * VDI Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VDI block storage
 */

#ifndef DIVDIBLOCKSTORAGE_H
#define DIVDIBLOCKSTORAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIBlockStorage.h"

typedef enum
{
    DI_VDI_DYNAMIC = 1,
    DI_VDI_STATIC,
} DIVDIFormat;

class DIVDIBlockStorage : public DIBlockStorage
{
public:
    DIVDIBlockStorage();
    
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
    
    DIInt vdiDataOffset;
    DIInt vdiBlockMapOffset;
    DIInt vdiBlockSize;
    DIInt vdiAllocatedBlockNum;
    
    vector<DIInt> vdiBlockMap;
    
    bool isBlockEmpty(const DIChar *buf);
    DIInt allocateVDIBlock(DIInt vdiBlockMapIndex);
};

#endif

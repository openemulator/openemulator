
/**
 * libdiskimage
 * VDI Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VDI block storage
 */

#include "DIVDIBlockStorage.h"

#define VDI_HEADERSIZE 0x200

#define VDI_EMPTY   0xffffffff

DIVDIBlockStorage::DIVDIBlockStorage()
{
    close();
}

bool DIVDIBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    DIChar header[VDI_HEADERSIZE];
    
    if (!backingStore->read(0, header, VDI_HEADERSIZE))
        return false;
    
    // Check VDI id
    if (getDIIntLE(&header[0x40]) != 0xbeda107f)
        return false;
    
    // Check VDI version
    if (getDIIntLE(&header[0x44]) != 0x00010001)
        return false;
    
    // Check VDI type
    DIInt type = getDIIntLE(&header[0x4c]);
    if ((type != DI_VDI_DYNAMIC) &&
        (type != DI_VDI_STATIC))
        return false;
    
    // Get image location
    vdiBlockMapOffset = getDIIntLE(&header[0x154]);
    vdiDataOffset = getDIIntLE(&header[0x158]);
    
    // Get CHS data
    cylinders = getDIIntLE(&header[0x15c]);
    heads = getDIIntLE(&header[0x160]);
    sectors = getDIIntLE(&header[0x164]);
    
    // Check sector size
    if (getDIIntLE(&header[0x168]) != DI_BLOCKSIZE)
        return false;
    
    // Get block num and VDI block size
    DILong diskSize = getDILongLE(&header[0x170]);
    if (diskSize % DI_BLOCKSIZE)
        return false;
    blockNum = (DIInt) (diskSize / DI_BLOCKSIZE);
    
    DIInt blockSize = getDIIntLE(&header[0x178]);
    if (blockSize % DI_BLOCKSIZE)
        return false;
    vdiBlockSize = blockSize / DI_BLOCKSIZE;
    
    DIInt vdiBlockNum = (blockNum + vdiBlockSize - 1) / vdiBlockSize;
    
    // Is link UUID zero?
    if (getDILongLE(&header[0x1a8]) || getDILongLE(&header[0x1b0]))
        return false;
    
    // Is parent UUID zero?
    if (getDILongLE(&header[0x1b8]) || getDILongLE(&header[0x1c0]))
        return false;
    
    // Read image block map
    DIData blockMap;
    
    blockMap.resize(sizeof(DIInt) * vdiBlockNum);
    if (!backingStore->read(vdiBlockMapOffset, &blockMap.front(), (DIInt) blockMap.size()))
        return false;
    
    vdiBlockMap.resize(blockNum / vdiBlockSize);
    for (DIInt i = 0; i < vdiBlockMap.size(); i++)
        vdiBlockMap[i] = getDIIntLE(&blockMap[i * sizeof(DIInt)]);
    
    // Get number of allocated blocks
    vdiAllocatedBlockNum = 0;
    
    for (vector<DIInt>::iterator i = vdiBlockMap.begin();
         i != vdiBlockMap.end();
         i++)
    {
        if (*i != VDI_EMPTY)
            vdiAllocatedBlockNum++;
    }
    
    this->backingStore = backingStore;
    
    return true;
}

void DIVDIBlockStorage::close()
{
    backingStore = NULL;
    
    blockNum = 0;
    
    cylinders = 0;
    heads = 0;
    sectors = 0;
    
    vdiBlockMapOffset = 0;
    vdiDataOffset = 0;
    vdiBlockSize = 0;
    vdiAllocatedBlockNum = 0;
    
    vdiBlockMap.clear();
}

bool DIVDIBlockStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIInt DIVDIBlockStorage::getBlockNum()
{
    return blockNum;
}

string DIVDIBlockStorage::getFormatLabel()
{
    string formatLabel = "VDI Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

DIInt DIVDIBlockStorage::getCylinders()
{
    return cylinders;
}

DIInt DIVDIBlockStorage::getHeads()
{
    return heads;
}

DIInt DIVDIBlockStorage::getSectors()
{
    return sectors;
}

bool DIVDIBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    for (; num; index++, buf += DI_BLOCKSIZE, num--)
    {
        if (index >= blockNum)
            return false;
        
        DIInt vdiBlockMapIndex = index / vdiBlockSize;
        DIInt vdiBlockIndex = vdiBlockMap[vdiBlockMapIndex];
        
        if (vdiBlockIndex == VDI_EMPTY)
            memset(buf, 0, DI_BLOCKSIZE);
        else            
        {
            DILong pos = (vdiDataOffset + (index + (vdiBlockIndex - vdiBlockMapIndex) *
                                           vdiBlockSize) * DI_BLOCKSIZE);
            
            if (!backingStore->read(pos, buf, DI_BLOCKSIZE))
                return false;
        }
    }
    
    return true;
}

bool DIVDIBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    for (; num; index++, buf += DI_BLOCKSIZE, num--)
    {
        if (index >= blockNum)
            return false;
        
        DIInt vdiBlockMapIndex = index / vdiBlockSize;
        DIInt vdiBlockIndex = vdiBlockMap[vdiBlockMapIndex];
        
        if (vdiBlockIndex == VDI_EMPTY)
        {
            if (isBlockEmpty(buf))
                continue;
            
            vdiBlockIndex = allocateVDIBlock(vdiBlockMapIndex);
            
            if (vdiBlockIndex == VDI_EMPTY)
                return false;
        }
        
        DILong pos = (vdiDataOffset + (vdiBlockIndex * vdiBlockSize +
                                       index % vdiBlockSize) * DI_BLOCKSIZE);
        
        if (!backingStore->write(pos, buf, DI_BLOCKSIZE))
            return false;
    }
    
    return true;
}

bool DIVDIBlockStorage::isBlockEmpty(const DIChar *buf)
{
    for (DIInt i = 0; i < DI_BLOCKSIZE; i++)
        if (buf[i])
            return false;
    
    return true;
}

DIInt DIVDIBlockStorage::allocateVDIBlock(DIInt vdiBlockMapIndex)
{
    // Allocate new block
    vdiAllocatedBlockNum++;
    
    DIData dummy;
    
    dummy.resize(vdiBlockSize);
    
    if (!backingStore->write(vdiDataOffset +
                             (vdiAllocatedBlockNum - 1) * vdiBlockSize * DI_BLOCKSIZE,
                             &dummy.front(), (DIInt) dummy.size()))
        return VDI_EMPTY;
    
    // Update block map
    DIChar intLEValue[4];
    
    setDIIntLE(intLEValue, vdiAllocatedBlockNum - 1);
    
    if (!backingStore->write(vdiBlockMapOffset + vdiBlockMapIndex * sizeof(DIInt),
                             intLEValue, sizeof(DIInt)))
        return VDI_EMPTY;
    
    setDIIntLE(intLEValue, vdiAllocatedBlockNum);
    
    if (!backingStore->write(0x184,
                             intLEValue, sizeof(DIInt)))
        return VDI_EMPTY;
    
    vdiBlockMap[vdiBlockMapIndex] = (vdiAllocatedBlockNum - 1);
    
    return vdiAllocatedBlockNum;
}

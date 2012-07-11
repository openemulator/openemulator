
/**
 * libdiskimage
 * VMDK Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VMDK block storage
 */

#include <sstream>

#include "DIVMDKBlockStorage.h"

#define VMDK_HEADERSIZE 0x200

DIVMDKBlockStorage::DIVMDKBlockStorage()
{
    backingStore = NULL;
    
    close();
}

bool DIVMDKBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    DIChar header[VMDK_HEADERSIZE];
    
    if (!backingStore->read(0, header, VMDK_HEADERSIZE))
        return false;
    
    // Check VMDK id
    if (getDIIntLE(&header[0x0]) != 0x564d444b)
        return false;
    
    // Check VMDK version
    DIInt version = getDIIntLE(&header[0x4]);
    if ((version != 1) && (version != 2))
        return false;
    
    // Get VMDK flags
    DIInt flags = getDIIntLE(&header[0x8]);
    
    redundantDirectory = (flags & (1 << 1));
    
    // Get VMDK capacity
    blockNum = (DIInt) getDILongLE(&header[0xc]);
    
    // Get VMDK grain info
    grainSize = (DIInt) getDILongLE(&header[0x14]);
    grainTableSize = getDIIntLE(&header[0x2c]);
    directory1Block = (DIInt) getDILongLE(&header[0x38]);
    directory2Block = (DIInt) getDILongLE(&header[0x30]);
    DIInt dataBlock = (DIInt) getDILongLE(&header[0x40]);
    
    // Get descriptor
    DILong descriptorOffset = getDILongLE(&header[0x1c]);
    DILong descriptorSize = getDILongLE(&header[0x24]);
    
    if (!parseDescriptor(backingStore, descriptorOffset, (DIInt) descriptorSize))
        return false;
    
    // Do not accept compression
    if ((flags & (1 << 16)) && (getDIShortLE(&header[0x4d]) != 0))
        return false;
    
    // Verify clean shutdown
    if (header[0x48])
        return false;
    
    // Read metadata
    DIData data;
    
    data.resize(dataBlock * DI_BLOCKSIZE);
    
    if (!backingStore->read(0, &data.front(), (DIInt) data.size()))
        return false;
    
    metadata.resize(data.size() / sizeof(DIInt));
    for (DIInt i = 0; i < metadata.size(); i++)
        metadata[i] = getDIIntLE(&data[i * sizeof(DIInt)]);
    
    directoryEntrySize = grainTableSize * grainSize;
    
    // Mark in use
    setInUse(true);
    
    this->backingStore = backingStore;
    
    return true;
}

void DIVMDKBlockStorage::close()
{
    // Unmark in use
    setInUse(false);
    
    backingStore = NULL;
    
    blockNum = 0;
    
    cylinders = 0;
    heads = 0;
    sectors = 0;
    
    grainSize = 0;
    grainTableSize = 0;
    directory1Block = 0;
    directory2Block = 0;
    redundantDirectory = false;
    allocatedBlockNum = 0;
}

bool DIVMDKBlockStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIInt DIVMDKBlockStorage::getBlockNum()
{
    return blockNum;
}

string DIVMDKBlockStorage::getFormatLabel()
{
    string formatLabel = "VMDK Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

DIInt DIVMDKBlockStorage::getCylinders()
{
    return cylinders;
}

DIInt DIVMDKBlockStorage::getHeads()
{
    return heads;
}

DIInt DIVMDKBlockStorage::getSectors()
{
    return sectors;
}

bool DIVMDKBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    for (; num; index++, buf += DI_BLOCKSIZE, num--)
    {
        DIInt directoryIndex = index / directoryEntrySize;
        DIInt directoryEntry = metadata[directory1Block * DI_BLOCKSIZE / sizeof(DIInt) +
                                        directoryIndex];
        
        DIInt grainTableIndex = (index % directoryEntrySize) / grainSize;
        DIInt grainTableEntry = metadata[directoryEntry * DI_BLOCKSIZE / sizeof(DIInt) +
                                         grainTableIndex];
        
        if (grainTableEntry <= 1)
            memset(buf, 0, DI_BLOCKSIZE);
        else            
        {
            DIInt blockIndex = grainTableEntry + index % grainSize;
            
            if (!backingStore->read(blockIndex * DI_BLOCKSIZE, buf, DI_BLOCKSIZE))
                return false;
        }
    }
    
    return true;
}

bool DIVMDKBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    for (; num; index++, buf += DI_BLOCKSIZE, num--)
    {
        DIInt directoryIndex = index / directoryEntrySize;
        DIInt directoryEntry = metadata[directory1Block * DI_BLOCKSIZE / sizeof(DIInt) +
                                        directoryIndex];
        
        DIInt grainTableIndex = (index % directoryEntrySize) / grainSize;
        DIInt grainTableEntry = metadata[directoryEntry * DI_BLOCKSIZE / sizeof(DIInt) +
                                         grainTableIndex];
        
        if (grainTableEntry <= 1)
        {
            if (isBlockEmpty(buf))
                continue;
            
            grainTableEntry = allocateGrain(directoryIndex, grainTableIndex);
            
            if (!grainTableEntry)
                return false;
        }
        
        DIInt blockIndex = grainTableEntry + index % grainSize;
        
        if (!backingStore->write(blockIndex * DI_BLOCKSIZE, buf, DI_BLOCKSIZE))
            return false;
    }
    
    return true;
}

bool DIVMDKBlockStorage::parseDescriptor(DIBackingStore *backingStore,
                                         DILong offset, DIInt size)
{
    DIData data;
    
    if (!size)
        return true;
    
    data.resize(size * DI_BLOCKSIZE);
    
    if (!backingStore->read(offset * DI_BLOCKSIZE, &data.front(), size))
        return false;
    
    string s((const char *) &data.front(), data.size());
    stringstream ss(s);
    
    // Process lines
    string line;
    
    while (!ss.eof())
    {
        string line;
        
        getline(ss, line);
        
        // Disregard empty lines
        if (!line.size())
            continue;
        
        // Remove comments
        if (line[0] == '#')
            continue;
        
        // Process line
        vector<string> tokens = strDISplit(line, '=');
        
        if (tokens.size() < 2)
            continue;
        
        string key = trimDI(tokens[0]);
        string value = strDIExcludeFilter(trimDI(tokens[1]), " ");
        
        // Process dictionary
        if (key == "version")
        {
            if (value != "1")
                return false;
        }
        else if (key == "ddb.geometry.sectors")
            sectors = getDIInt(value);
        else if (key == "ddb.geometry.heads")
            heads = getDIInt(value);
        else if (key == "ddb.geometry.cylinders")
            cylinders = getDIInt(value);
        else if (key == "parentCID")
        {
            if (value != "ffffffff")
                return false;
        }
    }
    
    return true;
}

bool DIVMDKBlockStorage::setInUse(bool value)
{
    if (!backingStore)
        return false;
    
    DIChar uncleanShutdown = value;
    
    return backingStore->write(0x48, &uncleanShutdown, 1);
}

bool DIVMDKBlockStorage::isBlockEmpty(const DIChar *buf)
{
    for (DIInt i = 0; i < DI_BLOCKSIZE; i++)
        if (buf[i])
            return false;
    
    return true;
}

DIInt DIVMDKBlockStorage::allocateGrain(DIInt directoryIndex, DIInt grainTableIndex)
{
    // Allocate new block
    DILong size = backingStore->getSize();
    
    DIInt newGrainTableEntry = (DIInt) ((size + DI_BLOCKSIZE - 1) / DI_BLOCKSIZE);
    
    DIData dummy;
    
    dummy.resize(grainSize * DI_BLOCKSIZE);
    
    if (!backingStore->write(newGrainTableEntry * DI_BLOCKSIZE,
                             &dummy.front(), (DIInt) dummy.size()))
        return 0;
    
    // Update grain table
    DIInt directoryEntry = metadata[directory1Block * DI_BLOCKSIZE / sizeof(DIInt) +
                                     directoryIndex];
    
    DIChar intLEValue[4];
    
    setDIIntLE(intLEValue, newGrainTableEntry);
    
    if (!backingStore->write(directoryEntry * DI_BLOCKSIZE + grainTableIndex * sizeof(DIInt),
                             intLEValue, sizeof(DIInt)))
        return 0;
    
    metadata[directoryEntry * DI_BLOCKSIZE / sizeof(DIInt) +
             grainTableIndex] = newGrainTableEntry;
    
    // Update redundant grain table
    if (redundantDirectory)
    {
        DIInt directoryEntry = metadata[directory2Block * DI_BLOCKSIZE / sizeof(DIInt) +
                                        directoryIndex];
        
        DIChar intLEValue[4];
        
        setDIIntLE(intLEValue, newGrainTableEntry);
        
        if (!backingStore->write(directoryEntry * DI_BLOCKSIZE + grainTableIndex * sizeof(DIInt),
                                 intLEValue, sizeof(DIInt)))
            return 0;
        
        metadata[directoryEntry * DI_BLOCKSIZE / sizeof(DIInt) +
                 grainTableIndex] = newGrainTableEntry;
        
    }
    
    return newGrainTableEntry;
}

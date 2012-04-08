
/**
 * libdiskimage
 * Block RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for handling raw disk images
 */

#include <sstream>

#include "BlockRAW.h"

BlockRAW::BlockRAW() : BlockData()
{
}

BlockRAW::BlockRAW(string path) : BlockData(path)
{
}

BlockRAW::BlockRAW(DIData& data) : BlockData(data)
{
}

BlockRAW::~BlockRAW()
{
}

bool BlockRAW::open(string path)
{
    if (!BlockData::open(path))
        return false;
    
    // Only accept sector-sized disk images
    if (dataSize & 0xff)
        return false;
    
    string ext = getDIPathExtension(path);
    
    if ((ext == "dsk") || (ext == "do"))
        sectorOrder = BLOCKRAW_APPLEDOS33;
    else if (ext == "cpm")
        sectorOrder = BLOCKRAW_APPLECPM;
    
    return true;
}

bool BlockRAW::open(DIData &data)
{
    if (!BlockData::open(data))
        return false;
    
    // Only accept sector-sized disk images
    if (dataSize & 0xff)
        return false;
    
    return true;
}

bool BlockRAW::read(DILong offset, DIData& data)
{
    if (offset & 0xff)
        return false;
    
    if (data.size() & 0xff)
        return false;
    
    if (!sectorOrder)
        return BlockData::read(offset, data);
    
    int sectorStart = offset >> 8;
    int sectorNum = data.size() >> 8;
    for (DIInt i = 0; i < sectorNum; i++)
    {
//        DIInt sector = 
//        if (!BlockData::read())
//            return false;
    }
    
    return true;
}

bool BlockRAW::write(DILong offset, DIData& data)
{
    if (offset & 0xff)
        return false;
    
    if (data.size() & 0xff)
        return false;
    
    return BlockData::write(offset, data);
}

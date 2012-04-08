
/**
 * libdiskimage
 * DiskCopy 4.2 Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Decodes the DiskCopy 4.2 format
 */

#include <string.h>

#include "BlockDC42.h"

BlockDC42::BlockDC42() : BlockData()
{
}

BlockDC42::BlockDC42(string path) : BlockData(path)
{
}

BlockDC42::BlockDC42(DIData& data) : BlockData(data)
{
}

BlockDC42::~BlockDC42()
{
}

bool BlockDC42::open(string path)
{
    if (!BlockData::open(path))
        return false;
    
    if (!openDC42())
    {
        close();
        
        return false;
    }
    
    return true;
}

bool BlockDC42::open(DIData& data)
{
    if (!BlockData::open(data))
        return false;
    
    if (!openDC42())
    {
        close();
        
        return false;
    }
    
    return true;
}

void BlockDC42::close()
{
    BlockData::close();
    
    imageOffset = 0;
    imageSize = 0;
    
    tagOffset = 0;
    tagSize = 0;
    
    gcrFormat = 0;
}

bool BlockDC42::openDC42()
{
    DIData header;
    
    header.resize(0x54);
    
    if (!BlockData::read(0, header))
        return false;
    
    // Get image and tag location
    imageOffset = 0x54;
    imageSize = getDIIntBE(&header[0x40]);
    
    tagOffset = imageOffset + imageSize;
    tagSize = getDIIntBE(&header[0x44]);
    
    if ((tagOffset + tagSize) > dataSize)
        return false;
    
    // Get GCR Format byte
    gcrFormat = header[0x51];
    
    // Check id
    if (getDIShortBE(&header[0x52]) != 0x0100)
        return false;
    
    return true;
}

bool BlockDC42::getProperty(string name, string& value)
{
    if (name == "gcrFormat")
        value = getDIString(gcrFormat);
    else
        return BlockData::getProperty(name, value);
    
    return true;
}

bool BlockDC42::read(DILong offset, DIData& data)
{
    if ((offset + data.size()) > imageSize)
        return false;
    
    return BlockData::read(imageOffset + offset, data);
}

bool BlockDC42::write(DILong offset, DIData& data)
{
    if ((offset + data.size()) > imageSize)
        return false;
    
    return BlockData::write(imageOffset + offset, data);
}

bool BlockDC42::readTag(DILong offset, DIData& data)
{
    if ((offset + data.size()) > tagSize)
        return false;
    
    return BlockData::read(tagOffset + offset, data);
}

bool BlockDC42::writeTag(DILong offset, DIData& data)
{
    if ((offset + data.size()) > tagSize)
        return false;
    
    return BlockData::write(tagOffset + offset, data);
}
